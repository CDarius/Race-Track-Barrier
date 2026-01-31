<template>
    <div class="field-container">
        <svg width="100%" height="100%" :viewBox="`0 0 ${fieldWidth} ${fieldHeight}`">
            <!-- Game Field Border -->
            <rect 
                :width="fieldWidth" 
                :height="fieldHeight" 
                :fill="fieldColor" />

            <!-- Left Paddle -->
            <rect 
                :x="0" 
                :y="leftPaddleY" 
                :width="paddleWidth " 
                :height="paddleHeight" 
                :fill="paddleColor" />

            <!-- Left Paddle Label -->
            <text
                :x="paddleWidth / 2"
                :y="leftPaddleY + paddleHeight / 2"
                text-anchor="middle"
                dominant-baseline="middle"
                :fill="paddleTextColor"
                :font-size="Math.max(paddleWidth, paddleHeight) * 0.6"
                font-family="Arial, sans-serif"
            >L</text>

            <!-- Right Paddle -->
            <rect 
                :x="fieldWidth - paddleWidth" 
                :y="rightPaddleY" 
                :width="paddleWidth" 
                :height="paddleHeight"
                :fill="paddleColor" />

            <!-- Right Paddle Label -->
            <text
                :x="fieldWidth - paddleWidth / 2"
                :y="rightPaddleY + paddleHeight / 2"
                text-anchor="middle"
                dominant-baseline="middle"
                :fill="paddleTextColor"
                :font-size="Math.max(paddleWidth, paddleHeight) * 0.6"
                font-family="Arial, sans-serif"
            >R</text>

            <!-- Ball -->
            <rect 
                :x="ballX" 
                :y="ballY" 
                :width="ballSide" 
                :height="ballSide" 
                :fill="ballColor" />
        </svg>
    </div>
</template>

<script setup lang="ts">
const fieldColor = '#222';
const paddleColor = '#00f';
const paddleTextColor = '#fff';
const ballColor = '#fff';

defineProps<{
    fieldWidth: number;
    fieldHeight: number;
    paddleWidth: number;
    paddleHeight: number;
    ballSide: number;
    leftPaddleY: number;
    rightPaddleY: number;
    ballX: number;
    ballY: number;
}>();

</script>

<style scoped>

.field-container {
    max-width: 100%;
    display: flex;
    box-sizing: border-box;
    /* Allow padding/margin to shrink content */
    border: 10px solid #e0e0e0; /* Light grey border */
}

svg {
    display: block;
    max-width: 100%;
    max-height: 100%;
    box-sizing: border-box;
    background: #222;
    margin: auto;
}
</style>
