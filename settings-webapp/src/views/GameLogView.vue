<template>
    <div v-if="showInitialLoading"
        class="h-100 w-100 d-flex flex-column justify-content-center align-items-center">
        <h3>Loading...</h3>
        <div class="spinner-border mt-2" style="width: 3rem; height: 3rem;" role="status">
            <span class="visually-hidden">Loading...</span>
        </div>
    </div>

    <div v-else-if="axesInfoStore.axesLoadingError" class="container-md">
        <div class="alert alert-danger d-flex align-items-center mt-4" role="alert">
            <i class="bi bi-exclamation-triangle-fill me-2"></i>
            <div>
                {{ axesInfoStore.axesLoadingError }}
            </div>
        </div>
    </div>

    <div v-else class="mx-3">
        <ErrorDialog title="Error" :message="errorDialogMessage" v-model:is-open="showErrorDialog">
        </ErrorDialog>

        <div class="d-grid mt-3">
            <button type="button" class="btn btn-primary" @click="downloadAndDisplayLastLog()"
                :disabled="downloadLogInProgress">
                Download last Log
            </button>            
        </div>

        <div v-if="!lastLog" class="container-md">
            <div class="alert alert-info d-flex align-items-center mt-4" role="alert">
                <i class="bi bi-exclamation-triangle-fill me-2"></i>
                <div>
                    No game log available. Please start a new game to create a log.
                </div>
            </div>
        </div>

        <div v-if="lastLog" class="position-relative mt-3" style="min-height: 200px;">
            <div>
                <PongGameField
                    :fieldWidth="fieldWidth + 2"
                    :fieldHeight="fieldHeight"
                    :paddleWidth="2"
                    :paddleHeight="3"
                    :ballSide="2"
                    :leftPaddleY="currentSimPositions.paddleL"
                    :rightPaddleY="currentSimPositions.paddleR"
                    :ballX="currentSimPositions.ballX"
                    :ballY="currentSimPositions.ballY"
                />

                <div class="mt-3 text-center">
                    <div class="btn-group" role="group" aria-label="Basic example">
                        <button type="button" class="btn btn-outline-dark"
                            @click="firstLogEntry()">
                            <i class="bi bi-skip-backward-fill"></i>
                        </button>
                        <button type="button" class="btn btn-outline-dark"
                            @click="previousLogEntry()">
                            <i class="bi bi-fast-forward-fill mirrored-icon"></i>
                        </button>                        
                        <button v-if="!isPlaying" type="button" class="btn btn-outline-dark"
                            @click="playLog()">
                            <i class="bi bi-play-fill"></i>
                        </button>
                        <button v-else type="button" class="btn btn-outline-dark"
                            @click="pauseLog()">
                            <i class="bi bi-pause-fill"></i>
                        </button>
                        <button type="button" class="btn btn-outline-dark"
                            @click="nextLogEntry()">
                            <i class="bi bi-fast-forward-fill"></i>
                        </button>
                        <button type="button" class="btn btn-outline-dark"
                            @click="lastLogEntry()">
                            <i class="bi bi-skip-forward-fill"></i>
                        </button>
                    </div>
                </div>
                <GameAxesPosTable class="mt-3"
                    :time="currentSimTime"
                    :ballX="currentSimPositions.ballX"
                    :ballY="currentSimPositions.ballY"
                    :paddleL="currentSimPositions.paddleL"
                    :paddleR="currentSimPositions.paddleR"
                />
                <GameAxesPosCalculusTable class="mt-2 mb-5"
                    :subCycles="currentSimSubCycles"
                />
            </div>
            <div v-if="downloadLogInProgress" class="overlay-spinner">
                <div class="spinner-border" style="width: 4rem; height: 4rem;" role="status">
                    <span class="visually-hidden">Loading...</span>
                </div>
            </div>
        </div>
    </div>
</template>

<script lang="ts" setup>

import { onBeforeMount, onBeforeUnmount, ref, computed } from 'vue';
import { useGameLogStore } from '@/stores/gameLog';
import { useAxesInfoStore } from '@/stores/axesInfo';

import PongGameField from '@/components/gamelog/PongGameField.vue';
import GameAxesPosTable from '@/components/gamelog/GameAxesPosTable.vue';
import GameAxesPosCalculusTable from '@/components/gamelog/GameAxesPosCalculusTable.vue';
import ErrorDialog from '@/components/dialogs/ErrorDialog.vue';

const gameLogStore = useGameLogStore();
const axesInfoStore = useAxesInfoStore();

const xAxisInfo = computed(() => axesInfoStore.axes.find(a => a.name.toLowerCase() === 'x'));
const yAxisInfo = computed(() => axesInfoStore.axes.find(a => a.name.toLowerCase() === 'y'));

const fieldWidth = computed(() => xAxisInfo.value?.sw_limit_p ?? 1);
const fieldHeight = computed(() => yAxisInfo.value?.sw_limit_p ?? 1);

const lastLog = computed(() => {
    if (gameLogStore.logs.length > 0)
        return gameLogStore.logs[gameLogStore.logs.length - 1];
    return null;
});

const showInitialLoading = computed(() => {
    return axesInfoStore.axesLoading || (!lastLog.value && downloadLogInProgress.value);    
});

const currentSimIndex = ref(0);
const currentSimEntry = computed(() => {
    if (!lastLog.value || lastLog.value.data.length === 0)
        return null;
    const index = Math.min(currentSimIndex.value, lastLog.value.data.length - 1);
    return lastLog.value.data[index];
});
const currentSimTime = computed(() => {
    if (!currentSimEntry.value)
        return 0;
    return currentSimEntry.value.timestamp;
});
const currentSimPositions = computed(() => {
    if (!currentSimEntry.value) {
         // If no log entry, position paddles and ball in center of field
         // (if field dimensions are known)
        if (xAxisInfo.value)
            return { ballX: fieldWidth.value / 2, ballY: fieldHeight.value / 2, paddleL: fieldHeight.value / 2, paddleR: fieldHeight.value / 2 };
        else
            return { ballX: 0, ballY: 0, paddleL: 0, paddleR: 0 };  
    }
    const entry = currentSimEntry.value.first_entry;
    return { ballX: entry.ballX, ballY: entry.ballY, paddleL: entry.paddleL, paddleR: entry.paddleR };
});
const currentSimSubCycles = computed(() => {
    if (!currentSimEntry.value)
        return [];
    return currentSimEntry.value.sub_entries;
});

const isPlaying = ref(false);

const downloadLogInProgress = ref(false);

const showErrorDialog = ref(false);
const errorDialogMessage = ref('');

let pollingActive = false;
let wasLogRunning = false;

onBeforeMount(() => {
    const init = async () => {
        if (axesInfoStore.axes == null || axesInfoStore.axes.length == 0)
            axesInfoStore.fetchAxesInfo();

        await downloadAndDisplayLastLog();

        pollingActive = true;
        monitorGameLogRunning();
    };

    init();
});

onBeforeUnmount(() => {
    pollingActive = false;
});

async function monitorGameLogRunning() {
    while (pollingActive) {
        let downloadNeeded = false;
        
        // Skip polling if a download is already in progress
        if (downloadLogInProgress.value) {
            await new Promise(resolve => setTimeout(resolve, 500));
            continue;
        }

        // Check if log is running
        try {
            const logRunning = await gameLogStore.isLogRunning();
            if (!logRunning && wasLogRunning) {
                downloadNeeded = true;
            }
            wasLogRunning = logRunning;
        } catch (error) {
            console.error("Error checking if game log is running:", error);
        }

        if (downloadNeeded) {
            await downloadAndDisplayLastLog();
        }

        await new Promise(resolve => setTimeout(resolve, 500));
    }
}

async function downloadAndDisplayLastLog() {
    downloadLogInProgress.value = true;
    console.log("Downloading last log...");

    try {
        currentSimIndex.value = 0;
        await gameLogStore.downloadLastLog();
    } catch (error) {
        errorDialogMessage.value = 'Error downloading game log: ' + (error instanceof Error ? error.message : String(error));
        showErrorDialog.value = true;
    } finally {
        downloadLogInProgress.value = false;
    }
}

function firstLogEntry() {
    if (!lastLog.value || lastLog.value.data.length === 0)
        return;
    currentSimIndex.value = 0;
}

function previousLogEntry() {
    if (!lastLog.value || lastLog.value.data.length === 0)
        return;
    currentSimIndex.value = Math.max(currentSimIndex.value - 1, 0);
}

function nextLogEntry() {
    if (!lastLog.value || lastLog.value.data.length === 0)
        return;
    currentSimIndex.value = Math.min(currentSimIndex.value + 1, lastLog.value.data.length - 1);
}

function lastLogEntry() {
    if (!lastLog.value || lastLog.value.data.length === 0)
        return;
    currentSimIndex.value = lastLog.value.data.length - 1;
}

function playLog() {
    if (!lastLog.value || lastLog.value.data.length === 0)
        return;

    let index = currentSimIndex.value;
    if (index >= lastLog.value.data.length - 1)
        index = 0;

    const startLogTime = lastLog.value.data[index].timestamp;
    const jsStartTime = performance.now();
    isPlaying.value = true;
    const playNextEntry = async () => {
        while (isPlaying.value) {
            await new Promise(resolve => setTimeout(resolve, 50));
    
            if (!lastLog.value || index >= lastLog.value.data.length - 1)
                return;
    
            const simTime = performance.now() - jsStartTime + startLogTime;
            while (index < lastLog.value.data.length - 1 && lastLog.value.data[index + 1].timestamp < simTime) {
                index++;
            }
            currentSimIndex.value = index;

            if (index >= lastLog.value.data.length - 1) {
                isPlaying.value = false;
                return;
            }
        }

    };

    playNextEntry();
}

function pauseLog() {
    isPlaying.value = false;
}

</script>

<style scoped>
.overlay-spinner {
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: rgba(255,255,255,0.5);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 10;
}
/* Mirrors icon horizontally */
.mirrored-icon {
    display: inline-block;
    transform: scaleX(-1);
}
</style>