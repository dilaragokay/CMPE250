

import javax.annotation.Generated;

@Generated("com.google.auto.value.processor.AutoValueProcessor")
final class AutoValue_Node extends Node {

  private final Coordinate coordinate;

  private final double g;

  AutoValue_Node(
      Coordinate coordinate,
      double g) {
    if (coordinate == null) {
      throw new NullPointerException("Null coordinate");
    }
    this.coordinate = coordinate;
    this.g = g;
  }

  @Override
  Coordinate coordinate() {
    return coordinate;
  }

  @Override
  double g() {
    return g;
  }

  @Override
  public String toString() {
    return "Node{"
         + "coordinate=" + coordinate + ", "
         + "g=" + g
        + "}";
  }

  @Override
  public boolean equals(Object o) {
    if (o == this) {
      return true;
    }
    if (o instanceof Node) {
      Node that = (Node) o;
      return (this.coordinate.equals(that.coordinate()))
           && (Double.doubleToLongBits(this.g) == Double.doubleToLongBits(that.g()));
    }
    return false;
  }

  @Override
  public int hashCode() {
    int h$ = 1;
    h$ *= 1000003;
    h$ ^= coordinate.hashCode();
    h$ *= 1000003;
    h$ ^= (int) ((Double.doubleToLongBits(g) >>> 32) ^ Double.doubleToLongBits(g));
    return h$;
  }

}
