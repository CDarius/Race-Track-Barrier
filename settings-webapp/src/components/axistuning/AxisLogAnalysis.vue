<template>
    <div class="row">
        <div class="col-md-3 col-lg-2">Movement Time: {{ movementTime }} ms</div>
        <div class="col-md-3 col-lg-2">Overshoot: {{ overshoot?.toFixed(2) }} studs</div>
        <div class="col-md-3 col-lg-2">Max Speed: {{ maxSpeed?.toFixed(2) }} studs/s</div>
        <div class="col-md-3 col-lg-2">Tau Time: {{ tauTime }} ms</div>
    </div>
</template>

<script setup lang="ts">

import { computed } from 'vue'
import type { AxisLogResponse } from '@/api/axesLogApi'
import type { AxisInfoResponse } from '@/api/axesInfoApi'
import { AxisLogColumns } from '@/api/axesLogApi'
import { getMovementTime, getOvershoot, getMaxSpeedFiltered, getTauTimeMs } from '@/components/axistuning/axisLogAnalysis.ts'

const props = defineProps<{
    log: AxisLogResponse,
    axisInfo: AxisInfoResponse
}>()

const movementTime = computed(() => 
    getMovementTime(props.log, 450, props.axisInfo.position_tolerance, props.axisInfo.standstill_speed));

const overshoot = computed(() => getOvershoot(props.log));
const maxSpeed = computed(() => getMaxSpeedFiltered(props.log, 20));
const tauTime = computed(() => getTauTimeMs(props.log, 20));

</script>