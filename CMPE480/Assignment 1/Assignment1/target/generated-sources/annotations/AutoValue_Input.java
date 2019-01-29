

import com.google.common.collect.ImmutableTable;
import javax.annotation.Generated;

@Generated("com.google.auto.value.processor.AutoValueProcessor")
final class AutoValue_Input extends Input {

  private final Coordinate startCoordinate;

  private final Coordinate endCoordinate;

  private final ImmutableTable<Integer, Integer, Double> world;

  private final int width;

  AutoValue_Input(
      Coordinate startCoordinate,
      Coordinate endCoordinate,
      ImmutableTable<Integer, Integer, Double> world,
      int width) {
    if (startCoordinate == null) {
      throw new NullPointerException("Null startCoordinate");
    }
    this.startCoordinate = startCoordinate;
    if (endCoordinate == null) {
      throw new NullPointerException("Null endCoordinate");
    }
    this.endCoordinate = endCoordinate;
    if (world == null) {
      throw new NullPointerException("Null world");
    }
    this.world = world;
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
  ImmutableTable<Integer, Integer, Double> world() {
    return world;
  }

  @Override
  int width() {
    return width;
  }

  @Override
  public String toString() {
    return "Input{"
         + "startCoordinate=" + startCoordinate + ", "
         + "endCoordinate=" + endCoordinate + ", "
         + "world=" + world + ", "
         + "width=" + width
        + "}";
  }

  @Override
  public boolean equals(Object o) {
    if (o == this) {
      return true;
    }
    if (o instanceof Input) {
      Input that = (Input) o;
      return (this.startCoordinate.equals(that.startCoordinate()))
           && (this.endCoordinate.equals(that.endCoordinate()))
           && (this.world.equals(that.world()))
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
    h$ ^= world.hashCode();
    h$ *= 1000003;
    h$ ^= width;
    return h$;
  }

}
