import com.google.auto.value.AutoValue;
import com.google.common.collect.ImmutableList;
import com.google.common.collect.ImmutableTable;

import java.io.*;
import java.util.*;

enum ProblemType {
    LABYRINTH, MOUNTAIN
}

enum OutputType {
    _a_star, _dfs, _bfs
}

public class Main {
    private static final String userDir = System.getProperty("user.dir");

    private static final ImmutableList<String> labyrinthInputFileNames = new ImmutableList.Builder<String>()
            .add("/labyrinth/labyrinth_test_1.txt")
            .add("/labyrinth/labyrinth_test_2.txt")
            .add("/labyrinth/labyrinth_test_3.txt")
            .add("/labyrinth/labyrinth_test_4.txt")
            .add("/labyrinth/labyrinth_test_5.txt")
            .build();

    private static final ImmutableList<String> mountainInputFileNames = new ImmutableList.Builder<String>()
            .add("/mountain/mountain_test_1.txt")
            .add("/mountain/mountain_test_2.txt")
            .add("/mountain/mountain_test_3.txt")
            .add("/mountain/mountain_test_4.txt")
            .add("/mountain/mountain_test_5.txt")
            .build();

    public static void main(String[] args) throws Exception {
        for (String fileName : labyrinthInputFileNames) {
            Input input = readFile(fileName);
            applyBFS(input, ProblemType.LABYRINTH, fileName);
            applyDFS(input, ProblemType.LABYRINTH, fileName);
            applyAstar(input, ProblemType.LABYRINTH, fileName);
        }
        for (String fileName : mountainInputFileNames) {
            Input input = readFile(fileName);
            applyBFS(input, ProblemType.MOUNTAIN, fileName);
            applyDFS(input, ProblemType.MOUNTAIN, fileName);
            applyAstar(input, ProblemType.MOUNTAIN, fileName);
        }
    }

    /**
     * Reads file.
     * <p>
     * Reads start coordinate, end coordinate and world matrix in the file respectively. Initiates an Input object
     * according to the information read.
     *
     * @return Input which has start coordinate, end coordinate, world matrix and the width of the world matrix
     */
    private static Input readFile(String fileName) throws Exception {
        File file = new File(userDir + "/src/main/resources/data/test" + fileName);
        BufferedReader br = new BufferedReader(new FileReader(file));
        String st;
        int startX = 0;
        int startY = 0;
        int endX = 0;
        int endY = 0;
        ImmutableTable.Builder<Integer, Integer, Double> worldBuilder = ImmutableTable.builder();
        // Read the start coordinate
        if ((st = br.readLine()) != null) {
            String[] numbers = st.split(" ");
            startX = Integer.parseInt(numbers[0]);
            startY = Integer.parseInt(numbers[1]);
        }
        // Read the end coordinate
        if ((st = br.readLine()) != null) {
            String[] numbers = st.split(" ");
            endX = Integer.parseInt(numbers[0]);
            endY = Integer.parseInt(numbers[1]);
        }
        // Read the world matrix
        int row = 0;
        while ((st = br.readLine()) != null) {
            int column = 0;
            String[] numbers = st.split(" ");
            for (String value : numbers) {
                worldBuilder.put(row, column, Double.parseDouble(value));
                column++;
            }
            row++;
        }
        ImmutableTable<Integer, Integer, Double> world = worldBuilder.build();
        int width = world.row(0).size();
        return Input.create(
                Coordinate.create(startX, startY),
                Coordinate.create(endX, endY),
                world,
                width);
    }

    /**
     * @param isDiscovered 2D array. An element in this array is 1 if the {@link Coordinate} which corresponds to this
     *                     index has been discovered during search.
     * @param path Path from end coordinate to start coordinate
     * @param distanceTraveled Total distance from start coordinate to end coordinate. Found from the {@code path}.
     * @param type labyrinth or mountain
     * @param outputType DFS, BFS or A*
     */
    private static void writeFile(
            int[][] isDiscovered,
            ImmutableList<Coordinate> path,
            String distanceTraveled,
            String fileName,
            ProblemType type,
            OutputType outputType) throws IOException {
        FileWriter fileWriter = new FileWriter(
                userDir
                        + "/output/"
                        + type.name().toLowerCase()
                        + "/"
                        + type.name().toLowerCase()
                        + "_test_"
                        + fileName.charAt(fileName.length() - 5)
                        + outputType.name()
                        + "_out.txt");
        PrintWriter printWriter = new PrintWriter(fileWriter);
        for (int[] row : isDiscovered) {
            printWriter.print(row[0]);
            for (int i = 1; i < row.length; i++) {
                printWriter.print(" " + row[i]);
            }
            printWriter.println();
        }
        printWriter.println(path.size());
        ListIterator<Coordinate> pathIterator = path.listIterator(path.size());
        while (pathIterator.hasPrevious()) {
            printWriter.println(pathIterator.previous());
        }
        printWriter.print(distanceTraveled);
        printWriter.close();
    }

    /**
     * Determines the legality of moving from the current coordinate to the neighboring one.
     *
     * If problem is a labyrinth, then move is legal if neighbor is not wall. If problem is mountain, then move is legal
     * if the height difference between the current coordinate and the neighbor coordinate is less than or equal to 1.
     * @param here Current coordinate
     * @param there Neighbor coordinate
     * @param type labyrinth or mountain. Used to determine legality of move
     * @param world Used to determine legality of move
     * @return true, if the neighbor can be added to frontier
     */
    private static boolean isMoveLegal(
            Coordinate here,
            Coordinate there,
            ProblemType type,
            ImmutableTable<Integer, Integer, Double> world) {
        if (type == ProblemType.LABYRINTH) {
            return world.get(there.x(), there.y()) == 0;
        } else {
            return Math.abs(world.get(there.x(), there.y()) - world.get(here.x(), here.y())) <= 1;
        }
    }

    /**
     * Computes distance traveled from the start coordinate to end coordinate.
     *
     * If problem is labyrinth, distance is path length minus one. If problem is mountain, distance is sum of Euclidean
     * distance between each consecutive coordinate in the path.
     *
     * @param path path from the start coordinate to end coordinate
     * @param type labyrinth or mountain
     * @param world used to compute distance
     * @return distance traveled from the start coordinate to end coordinate
     */
    private static String computeDistanceTraveled(ImmutableList<Coordinate> path,
                                                  ProblemType type,
                                                  ImmutableTable<Integer, Integer, Double> world) {
        if (type == ProblemType.LABYRINTH) {
            return (path.size() - 1) + ".00";
        } else {
            double totalDistance = 0;
            ListIterator<Coordinate> iterator = path.listIterator();
            Coordinate currentCoordinate = iterator.next();
            while (iterator.hasNext()) {
                Coordinate nextCoordinate = iterator.next();
                totalDistance += Math.sqrt(
                        Math.pow(world.get(nextCoordinate.x(), nextCoordinate.y()) - world.get(currentCoordinate.x(), currentCoordinate.y()), 2)
                                + 1);
                currentCoordinate = nextCoordinate;
            }
            return String.format("%.2f", totalDistance);
        }
    }

    /**
     * The algorithm (depth-first search) starts at the start coordinate and explores as far as possible along each
     * branch before backtracking. Search stops when end coordinate is found.
     */
    private static void applyDFS(Input input, ProblemType type, String fileName) throws IOException {
        Deque<Coordinate> stack = new ArrayDeque<>();
        Map<Coordinate, Coordinate> cameFrom = new HashMap<>();
        int[][] isDiscovered = new int[input.world().size() / input.width()][input.width()];
        stack.push(input.startCoordinate());
        while (!stack.peek().equals(input.endCoordinate())) {
            Coordinate currentCoordinate = stack.pop();
            isDiscovered[currentCoordinate.x()][currentCoordinate.y()] = 1;
            // If the world has a coordinate above the current one
            if (input.world().contains(currentCoordinate.x() - 1, currentCoordinate.y())) {
                Coordinate newCoordinate = Coordinate.create(currentCoordinate.x() - 1, currentCoordinate.y());
                // If the coordinate above is not a wall and it is not discovered yet
                if (isMoveLegal(currentCoordinate, newCoordinate, type, input.world())
                        && isDiscovered[currentCoordinate.x() - 1][currentCoordinate.y()] == 0) {
                    stack.push(newCoordinate);
                    cameFrom.putIfAbsent(newCoordinate, currentCoordinate);
                }
            }
            // If the world has a coordinate on left of the current one
            if (input.world().contains(currentCoordinate.x(), currentCoordinate.y() - 1)) {
                Coordinate newCoordinate = Coordinate.create(currentCoordinate.x(), currentCoordinate.y() - 1);
                // If the coordinate on the left is not a wall and it is not discovered yet
                if (isMoveLegal(currentCoordinate, newCoordinate, type, input.world())
                        && isDiscovered[currentCoordinate.x()][currentCoordinate.y() - 1] == 0) {
                    stack.push(newCoordinate);
                    cameFrom.putIfAbsent(newCoordinate, currentCoordinate);
                }
            }
            // If the world has a coordinate on right of the current one
            if (input.world().contains(currentCoordinate.x(), currentCoordinate.y() + 1)) {
                Coordinate newCoordinate = Coordinate.create(currentCoordinate.x(), currentCoordinate.y() + 1);
                // If the coordinate on the right is not a wall and it is not discovered yet
                if (isMoveLegal(currentCoordinate, newCoordinate, type, input.world())
                        && isDiscovered[currentCoordinate.x()][currentCoordinate.y() + 1] == 0) {
                    stack.push(newCoordinate);
                    cameFrom.putIfAbsent(newCoordinate, currentCoordinate);
                }
            }
            // If the world has a coordinate below the current one
            if (input.world().contains(currentCoordinate.x() + 1, currentCoordinate.y())) {
                Coordinate newCoordinate = Coordinate.create(currentCoordinate.x() + 1, currentCoordinate.y());
                // If the coordinate below is not a wall and it is not discovered yet
                if (isMoveLegal(currentCoordinate, newCoordinate, type, input.world())
                        && isDiscovered[currentCoordinate.x() + 1][currentCoordinate.y()] == 0) {
                    stack.push(newCoordinate);
                    cameFrom.putIfAbsent(newCoordinate, currentCoordinate);
                }
            }
        }
        isDiscovered[input.endCoordinate().x()][input.endCoordinate().y()] = 1;
        Coordinate currentCoordinate = input.endCoordinate();
        // Find the path from start coordinate to end coordinate in reverse order
        ImmutableList.Builder<Coordinate> path = ImmutableList.builder();
        while (currentCoordinate != input.startCoordinate()) {
            path.add(currentCoordinate);
            currentCoordinate = cameFrom.get(currentCoordinate);
        }
        ImmutableList<Coordinate> immutablePath = path.add(input.startCoordinate()).build();
        String distanceTraveled = computeDistanceTraveled(immutablePath, type, input.world());
        writeFile(isDiscovered, immutablePath, distanceTraveled, fileName, type, OutputType._dfs);
    }

    /**
     * The algorithm (breadth-first search) starts at the start coordinate and explores all of the neighbor nodes at the
     * present depth prior to moving on to the nodes at the next depth level.
     */
    private static void applyBFS(Input input, ProblemType type, String fileName) throws IOException {
        Queue<Coordinate> queue = new ArrayDeque<>();
        Map<Coordinate, Coordinate> cameFrom = new HashMap<>();
        int[][] isDiscovered = new int[input.world().size() / input.width()][input.width()];
        queue.add(input.startCoordinate());
        isDiscovered[input.startCoordinate().x()][input.startCoordinate().y()] = 1;
        while (!queue.peek().equals(input.endCoordinate())) {
            Coordinate currentCoordinate = queue.remove();
            isDiscovered[currentCoordinate.x()][currentCoordinate.y()] = 1;
            // If the world has a coordinate below the current one
            if (input.world().contains(currentCoordinate.x() + 1, currentCoordinate.y())) {
                Coordinate newCoordinate = Coordinate.create(currentCoordinate.x() + 1, currentCoordinate.y());
                // If the coordinate below is not a wall and it is not discovered yet
                if (isMoveLegal(currentCoordinate, newCoordinate, type, input.world())
                        && isDiscovered[currentCoordinate.x() + 1][currentCoordinate.y()] == 0) {
                    queue.add(newCoordinate);
                    cameFrom.putIfAbsent(newCoordinate, currentCoordinate);
                }
            }
            // If the world has a coordinate on right of the current one
            if (input.world().contains(currentCoordinate.x(), currentCoordinate.y() + 1)) {
                Coordinate newCoordinate = Coordinate.create(currentCoordinate.x(), currentCoordinate.y() + 1);
                // If the coordinate on the right is not a wall and it is not discovered yet
                if (isMoveLegal(currentCoordinate, newCoordinate, type, input.world())
                        && isDiscovered[currentCoordinate.x()][currentCoordinate.y() + 1] == 0) {
                    queue.add(newCoordinate);
                    cameFrom.putIfAbsent(newCoordinate, currentCoordinate);
                }
            }
            // If the world has a coordinate on left of the current one
            if (input.world().contains(currentCoordinate.x(), currentCoordinate.y() - 1)) {
                Coordinate newCoordinate = Coordinate.create(currentCoordinate.x(), currentCoordinate.y() - 1);
                // If the coordinate on the left is not a wall and it is not discovered yet
                if (isMoveLegal(currentCoordinate, newCoordinate, type, input.world())
                        && isDiscovered[currentCoordinate.x()][currentCoordinate.y() - 1] == 0) {
                    queue.add(newCoordinate);
                    cameFrom.putIfAbsent(newCoordinate, currentCoordinate);
                }
            }
            // If the world has a coordinate above the current one
            if (input.world().contains(currentCoordinate.x() - 1, currentCoordinate.y())) {
                Coordinate newCoordinate = Coordinate.create(currentCoordinate.x() - 1, currentCoordinate.y());
                // If the coordinate above is not a wall and it is not discovered yet
                if (isMoveLegal(currentCoordinate, newCoordinate, type, input.world())
                        && isDiscovered[currentCoordinate.x() - 1][currentCoordinate.y()] == 0) {
                    queue.add(newCoordinate);
                    cameFrom.putIfAbsent(newCoordinate, currentCoordinate);
                }
            }
        }
        isDiscovered[input.endCoordinate().x()][input.endCoordinate().y()] = 1;
        Coordinate currentCoordinate = input.endCoordinate();
        // Find the path from start coordinate to end coordinate in reverse order
        ImmutableList.Builder<Coordinate> path = ImmutableList.builder();
        while (currentCoordinate != input.startCoordinate()) {
            path.add(currentCoordinate);
            currentCoordinate = cameFrom.get(currentCoordinate);
        }
        ImmutableList<Coordinate> immutablePath = path.add(input.startCoordinate()).build();
        String distanceTraveled = computeDistanceTraveled(immutablePath, type, input.world());
        writeFile(isDiscovered, immutablePath, distanceTraveled, fileName, type, OutputType._bfs);
    }

    /**
     * Used in {@code applyAstar} method.
     * @param here current node
     * @param there neighbor coordinate
     * @param type labyrinth or mountain
     * @param givenCost the present cost of the neighbor coordinate
     * @return true, if the new cost of neighbor coordinate is less than its present cost
     */
    private static boolean isCostLess(
            Node here,
            Coordinate there,
            Coordinate endCoordinate,
            ProblemType type,
            ImmutableTable<Integer, Integer, Double> world,
            double givenCost) {
        if (type == ProblemType.LABYRINTH) {
            return here.g + 1 + Math.abs(there.y() - endCoordinate.y()) + Math.abs(there.x() - endCoordinate.x())
                    < givenCost;
        } else {
            return here.g
                    + Math.sqrt(1 + Math.pow(
                    world.get(there.x(), there.y()) - world.get(here.coordinate.x(), here.coordinate.y()), 2))
                    + Math.sqrt(
                    Math.pow(world.get(there.x(), there.y()) - world.get(endCoordinate.x(), endCoordinate.y()), 2)
                            + Math.pow(there.y() - endCoordinate.y(), 2)
                            + Math.pow(there.x() - endCoordinate.x(), 2)) < givenCost;
        }
    }

    /**
     * This algorithm (A* or A-star search) uses heuristics and distance from the start coordinate
     */
    private static void applyAstar(Input input, ProblemType type, String fileName) throws IOException {
        Queue<Node> priorityQueue = new PriorityQueue<>(Node.NODE_COMPARATOR.reversed());
        Map<Coordinate, Coordinate> cameFrom = new HashMap<>();
        Map<Coordinate, Double> cost = new HashMap<>();
        int[][] isDiscovered = new int[input.world().size() / input.width()][input.width()];
        priorityQueue.add(Node.create(
                input.startCoordinate(),
                input.startCoordinate().x() * input.width() + input.startCoordinate().y(),
                0,
                Math.abs(input.endCoordinate().y() - input.startCoordinate().y())
                        + Math.abs(input.endCoordinate().x() - input.startCoordinate().x())));
        cost.put(input.startCoordinate(), (double) (Math.abs(input.endCoordinate().y() - input.startCoordinate().y())
                + Math.abs(input.endCoordinate().x() - input.startCoordinate().x())));
        while (!priorityQueue.isEmpty() && !priorityQueue.peek().coordinate.equals(input.endCoordinate())) {
            Node currentNode = priorityQueue.remove();
            isDiscovered[currentNode.coordinate.x()][currentNode.coordinate.y()] = 1;
            ImmutableList.Builder<Coordinate> neighborsBuilder = ImmutableList.builder();
            if (input.world().contains(currentNode.coordinate.x() + 1, currentNode.coordinate.y())
                    && isMoveLegal(currentNode.coordinate, Coordinate.create(
                    currentNode.coordinate.x() + 1,
                    currentNode.coordinate.y()), type, input.world())) {
                neighborsBuilder.add(Coordinate.create(
                        currentNode.coordinate.x() + 1,
                        currentNode.coordinate.y()));
            }
            if (input.world().contains(currentNode.coordinate.x() - 1, currentNode.coordinate.y())
                    && isMoveLegal(currentNode.coordinate, Coordinate.create(
                    currentNode.coordinate.x() - 1,
                    currentNode.coordinate.y()), type, input.world())) {
                neighborsBuilder.add(Coordinate.create(
                        currentNode.coordinate.x() - 1,
                        currentNode.coordinate.y()));
            }
            if (input.world().contains(currentNode.coordinate.x(), currentNode.coordinate.y() - 1)
                    && isMoveLegal(currentNode.coordinate, Coordinate.create(
                    currentNode.coordinate.x(),
                    currentNode.coordinate.y() - 1), type, input.world())) {
                neighborsBuilder.add(Coordinate.create(
                        currentNode.coordinate.x(),
                        currentNode.coordinate.y() - 1));
            }
            if (input.world().contains(currentNode.coordinate.x(), currentNode.coordinate.y() + 1)
                    && isMoveLegal(currentNode.coordinate, Coordinate.create(
                    currentNode.coordinate.x(),
                    currentNode.coordinate.y() + 1), type, input.world())) {
                neighborsBuilder.add(Coordinate.create(
                        currentNode.coordinate.x(),
                        currentNode.coordinate.y() + 1));
            }
            ImmutableList<Coordinate> neighbors = neighborsBuilder.build();
            for (Coordinate newNeighbor : neighbors) {
                cost.putIfAbsent(newNeighbor, Double.MAX_VALUE);
                if (cost.get(newNeighbor) == Double.MAX_VALUE
                        || isCostLess(currentNode, newNeighbor, input.endCoordinate(), type, input.world(), cost.get(newNeighbor))) {
                    double h = 0;
                    double g = 0;
                    if (type == ProblemType.LABYRINTH) {
                        g = currentNode.g + 1;
                        h = Math.abs(newNeighbor.y() - input.endCoordinate().y()) + Math.abs(newNeighbor.x() - input.endCoordinate().x());
                    } else {
                        g = currentNode.g
                                + (double) Math.sqrt(
                                1 + Math.pow(input.world().get(newNeighbor.x(), newNeighbor.y()) - input.world().get(currentNode.coordinate.x(), currentNode.coordinate.y()), 2));
                        h = (double) Math.sqrt(
                                Math.pow(input.world().get(newNeighbor.x(), newNeighbor.y()) - input.world().get(input.endCoordinate().x(), input.endCoordinate().y()), 2)
                                        + Math.pow(newNeighbor.y() - input.endCoordinate().y(), 2)
                                        + Math.pow(newNeighbor.x() - input.endCoordinate().x(), 2));
                    }
                    priorityQueue.add(Node.create(newNeighbor, newNeighbor.x() * input.width() + newNeighbor.y(), g, h));
                    cost.put(newNeighbor, g + h);
                    cameFrom.put(newNeighbor, currentNode.coordinate);
                }
            }
        }
        isDiscovered[input.endCoordinate().x()][input.endCoordinate().y()] = 1;
        // Find the path from start coordinate to end coordinate in reverse order
        Coordinate currentCoordinate = input.endCoordinate();
        ImmutableList.Builder<Coordinate> path = ImmutableList.builder();
        while (currentCoordinate != input.startCoordinate()) {
            path.add(currentCoordinate);
            currentCoordinate = cameFrom.get(currentCoordinate);
        }
        ImmutableList<Coordinate> immutablePath = path.add(input.startCoordinate()).build();
        String distanceTraveled = computeDistanceTraveled(immutablePath, type, input.world());
        writeFile(isDiscovered, immutablePath, distanceTraveled, fileName, type, OutputType._a_star);
    }
}

@AutoValue
abstract class Input {
    static Input create(
            Coordinate startCoordinate,
            Coordinate endCoordinate,
            ImmutableTable<Integer, Integer, Double> world,
            int width) {
        return new AutoValue_Input(startCoordinate, endCoordinate, world, width);
    }

    abstract Coordinate startCoordinate();

    abstract Coordinate endCoordinate();

    abstract ImmutableTable<Integer, Integer, Double> world();

    abstract int width();
}

@AutoValue
abstract class Coordinate {
    static Coordinate create(int x, int y) {
        return new AutoValue_Coordinate(x, y);
    }

    @Override
    public String toString() {
        return x() + " " + y();
    }

    abstract int x();

    abstract int y();
}

class Node {
    static Comparator<Node> NODE_COMPARATOR = Comparator
            .comparingDouble((Node n) -> n.g + n.h).reversed()
            .thenComparingInt((Node n) -> n.index);
    Coordinate coordinate;
    int index;
    double g;
    double h;

    Node(Coordinate coordinate, int index, double g, double h) {
        this.coordinate = coordinate;
        this.index = index;
        this.g = g;
        this.h = h;
    }

    static Node create(Coordinate coordinate, int index, double g, double h) {
        return new Node(coordinate, index, g, h);
    }
}