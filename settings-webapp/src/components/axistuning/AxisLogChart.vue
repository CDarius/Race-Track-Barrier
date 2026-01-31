<template>
    <div>
        <Line :data="chartData" :options="chartOptions" />
    </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { Line } from 'vue-chartjs'
import {
    Chart,
    LineController,
    LineElement,
    PointElement,
    LinearScale,
    Title,
    CategoryScale,
    Legend,
    Tooltip,
} from 'chart.js'
import type { ChartOptions } from 'chart.js'
import type { AxisLogResponse } from '@/api/axesLogApi'
import { AxisLogColumns } from '@/api/axesLogApi'

Chart.register(LineController, LineElement, PointElement, LinearScale, Title, CategoryScale, Legend, Tooltip)

// Props
const props = withDefaults(defineProps<{
    log: AxisLogResponse,
    displayedColumns?: AxisLogColumns[]
}>(), {
    displayedColumns: () => [
        AxisLogColumns.ActualPosition, AxisLogColumns.ActualSpeed, AxisLogColumns.ActualPwmOutput,
        AxisLogColumns.PositionSetpoint, AxisLogColumns.SpeedSetpoint, AxisLogColumns.PositionError]
})

// Prepare chart data
const chartData = computed(() => {
    const colsIndexesToDisplay = (props.displayedColumns?.map(col => Number(col)) || [])
        .sort((a, b) => a - b)
    const { col_names, data } = props.log
    // Assume first column is time, rest are series
    const labels = data.map(row => row[0])
    const datasets = col_names.slice(1).map((name, idx) => ({
        label: name,
        data: data.map(row => row[idx + 1]),
        fill: false,
        borderColor: `hsl(${(idx * 50) % 360}, 70%, 50%)`,
        tension: 0.1,
        pointRadius: 0,
        // Assign last displayed column to secondary axis
        yAxisID: (idx === Number(AxisLogColumns.ActualPwmOutput)) ? 'y1' : 'y'

    }))
    // Only keep datasets where (idx + 1) is in colsIndexesToDisplay
    const filteredDatasets = datasets.filter((_, idx) => colsIndexesToDisplay.includes(idx))
    return {
        labels,
        datasets: filteredDatasets
    }
})

const chartOptions: ChartOptions<'line'> = {
    responsive: true,
    plugins: {
        legend: { display: true },
        title: { display: true, text: 'Axis Log Time Series' }
    },
    scales: {
        x: { title: { display: false, text: props.log.col_names[0] } },
        y: {
            title: { display: true, text: 'Axis Values' },
            position: 'left'
        },
        y1: {
            title: { display: true, text: 'PWM' },
            position: 'right',
            grid: { drawOnChartArea: false }
        }
    }
}
</script>