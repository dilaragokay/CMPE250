

import com.google.common.collect.ImmutableTable;
import javax.annotation.Generated;

@Generated("com.google.auto.value.processor.AutoValueProcessor")
final class AutoValue_LabyrinthInput extends LabyrinthInput {

  private final Coordinate startCoordinate;

  private final Coordinate endCoordinate;

  private final ImmutableTable<Integer, Integer, Integer> labyrinth;

  private final int width;

  AutoValue_LabyrinthInput(
      Coordinate startCoordinate,
      Coordinate endCoordinate,
      ImmutableTable<Integer, Integer, Integer> labyrinth,
      int width) {
    if (startCoordinate == null) {
      throw new NullPointerException("Null startCoordinate");
    }
    this.startCoordinate = startCoordinate;
    if (endCoordinate == null) {
      throw new NullPointerException("Null endCoordinate");
    }
    this.endCoordinate = endCoordinate;
    if (labyrinth == null) {
      throw new NullPointerException("Null labyrinth");
    }
    this.labyrinth = labyrinth;
    this.width = width;
  }

  @Override
  Coordinate startCoordinate() {
    return startCoordinate;
  }

  @Override
  Coordinate endCoordinate() {
    return endCoordinate;
  }

  @Override
  ImmutableTable<Integer, Integer, Integer> labyrinth() {
    return labyrinth;
  }

  @Override
  int width() {
    return width;
  }

  @Override
  public String toString() {
    return "LabyrinthInput{"
         + "startCoordinate=" + startCoordinate + ", "
         + "endCoordinate=" + endCoordinate + ", "
         + "labyrinth=" + labyrinth + ", "
         + "width=" + width
        + "}";
  }

  @Override
  public boolean equals(Object o) {
    if (o == this) {
      return true;
    }
    if (o instanceof LabyrinthInput) {
      LabyrinthInput that = (LabyrinthInput) o;
      return (this.startCoordinate.equals(that.startCoordinate()))
           && (this.endCoordinate.equals(that.endCoordinate()))
           && (this.labyrinth.equals(that.labyrinth()))
           && (this.width == that.width());
    }
    return false;
  }

  @Override
  public int hashCode() {
    int h$ = 1;
    h$ *= 1000003;
    h$ ^= startCoordinate.hashCode();
    h$ *= 1000003;
    h$ ^= endCoordinate.hashCode();
    h$ *= 1000003;
    h$ ^= labyrinth.hashCode();
    h$ *= 1000003;
    h$ ^= width;
    return h$;
  }

}
