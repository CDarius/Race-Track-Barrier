export type AxesInfoResponse = AxisInfoResponse[];

export interface AxisInfoResponse {
    name: string;
    counts_per_unit: number;
    standstill_speed: number;
    position_tolerance: number;
    sw_limit_m: number;
    sw_limit_p: number;
}
