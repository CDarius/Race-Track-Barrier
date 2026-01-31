import type { AxisLogResponse } from '@/api/axesLogApi'
import { AxisLogColumns } from '@/api/axesLogApi'

export function getLogTimeMs(log: AxisLogResponse, row: number): number {
    // Time is always in column 0
    return log.data[row][0];
}

export function getColumnValue(log: AxisLogResponse, row: number, column: AxisLogColumns): number {
    return log.data[row][Number(column) + 1]; // +1 because time is column 0
}

export function getMovementStartIndex(log: AxisLogResponse): number | undefined {
    for (let i = 0; i < log.rows; i++) {
        if (getColumnValue(log, i, AxisLogColumns.ActuationType) == 3) {
            return i;
        }
    }

    return undefined;
}

export function getMovementTime(log: AxisLogResponse, settleTimeMs: number, positionTolerance: number, standStillSpeed: number): number | undefined {
    const startIndex = getMovementStartIndex(log);
    if (startIndex === undefined) 
        return undefined;

    const targetPosition = getColumnValue(log, startIndex, AxisLogColumns.PositionSetpoint);

    let beginOnTargetIndex: number = 0;
    for (let i = startIndex; i < log.rows; i++) {
        const actualPosition = getColumnValue(log, i, AxisLogColumns.ActualPosition);
        const actualSpeed = getColumnValue(log, i, AxisLogColumns.ActualSpeed);
        const onTarget = Math.abs(actualPosition - targetPosition) <= positionTolerance 
            && Math.abs(actualSpeed) <= standStillSpeed;

        if (onTarget) {
            if (beginOnTargetIndex == 0) {
                beginOnTargetIndex = i;
            }
            else {
                const startOnTargetTime = getLogTimeMs(log, beginOnTargetIndex);
                const currentTime = getLogTimeMs(log, i);
                if ((currentTime - startOnTargetTime) >= settleTimeMs) {
                    return startOnTargetTime - getLogTimeMs(log, startIndex);
                }
            }
        } else {
            beginOnTargetIndex = 0;
        }
    }

    return undefined;
}

export function getMaxSpeed(log: AxisLogResponse): number {
    let maxSpeed = 0;
    for (let i = 0; i < log.rows; i++) {
        const actualSpeed = Math.abs(getColumnValue(log, i, AxisLogColumns.ActualSpeed));
        if (actualSpeed > maxSpeed) {
            maxSpeed = actualSpeed;
        }
    }
    return maxSpeed;
}

// Computes the max speed using the average of numSamples consecutive ActualSpeed values
export function getMaxSpeedFiltered(log: AxisLogResponse, numSamples: number): number {
    if (numSamples <= 1) {
        return getMaxSpeed(log);
    }
    let maxAvgSpeed = 0;
    for (let i = 0; i <= log.rows - numSamples; i++) {
        let sum = 0;
        for (let j = 0; j < numSamples; j++) {
            sum += Math.abs(getColumnValue(log, i + j, AxisLogColumns.ActualSpeed));
        }
        const avgSpeed = sum / numSamples;
        if (avgSpeed > maxAvgSpeed) {
            maxAvgSpeed = avgSpeed;
        }
    }
    return maxAvgSpeed;
}

export function getTauTimeMs(log: AxisLogResponse, numSamples: number): number | undefined {
    const startIndex = getMovementStartIndex(log);
    if (startIndex === undefined) 
        return undefined;

    const maxSpeed = getMaxSpeedFiltered(log, numSamples);
    const tauSpeed = maxSpeed * 0.63;
    for (let i = startIndex; i < log.rows; i++) {
        const actualSpeed = Math.abs(getColumnValue(log, i, AxisLogColumns.ActualSpeed));
        if (actualSpeed >= tauSpeed) {
            return getLogTimeMs(log, i) - getLogTimeMs(log, startIndex);
        }
    }

    return undefined;
}

export function getOvershoot(log: AxisLogResponse): number | undefined {
    const startIndex = getMovementStartIndex(log);
    if (startIndex === undefined)
        return undefined;

    const targetPosition = getColumnValue(log, startIndex, AxisLogColumns.PositionSetpoint);
    let overshoot = 0;
    for (let i = startIndex; i < log.rows; i++) {
        const actualPosition = getColumnValue(log, i, AxisLogColumns.ActualPosition);
        if (actualPosition > targetPosition) {
            const newOvershoot = actualPosition - targetPosition;
            if (newOvershoot > overshoot) {
                overshoot = newOvershoot;
            }
        }
    }

    return overshoot;
}
