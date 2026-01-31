export interface AxisLogResponse {
    rows: number;
    cols: number;
    col_names: string[];
    col_units: string[];
    data: number[][];
}

export enum AxisLogColumns {
  ActualPosition = 1,
  ActualSpeed = 2,
  ActuationType = 3,
  ActualPwmOutput = 4,
  PositionSetpoint = 5,
  SpeedSetpoint = 6,
  PositionError = 7,
}