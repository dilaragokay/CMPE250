

import javax.annotation.Generated;

@Generated("com.google.auto.value.processor.AutoValueProcessor")
final class AutoValue_StartEndCoordinate extends StartEndCoordinate {

  private final int x;

  private final int y;

  AutoValue_StartEndCoordinate(
      int x,
      int y) {
    this.x = x;
    this.y = y;
  }

  @Override
  int x() {
    return x;
  }

  @Override
  int y() {
    return y;
  }

  @Override
  public String toString() {
    return "StartEndCoordinate{"
         + "x=" + x + ", "
         + "y=" + y
        + "}";
  }

  @Override
  public boolean equals(Object o) {
    if (o == this) {
      return true;
    }
    if (o instanceof StartEndCoordinate) {
      StartEndCoordinate that = (StartEndCoordinate) o;
      return (this.x == that.x())
           && (this.y == that.y());
    }
    return false;
  }

  @Override
  public int hashCode() {
    int h$ = 1;
    h$ *= 1000003;
    h$ ^= x;
    h$ *= 1000003;
    h$ ^= y;
    return h$;
  }

}
