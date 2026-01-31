<template>
    <div v-bind="$attrs">
        <ErrorDialog title="Error" :message="errorDialogMessage" v-model:is-open="showErrorDialog">
        </ErrorDialog>

        <div v-if="loading" class="h-100 w-100 d-flex flex-column justify-content-center align-items-center">
            <h3>Loading...</h3>
            <div class="spinner-border mt-2" style="width: 3rem; height: 3rem;" role="status">
                <span class="visually-hidden">Loading...</span>
            </div>
        </div>

        <div v-else-if="apiError" class="container-md">
            <div class="alert alert-danger d-flex align-items-center mt-4" role="alert">
                <i class="bi bi-exclamation-triangle-fill me-2"></i>
                <div>
                    {{ apiError }}
                </div>
            </div>
        </div>

        <div v-if="prerequisites.length > 0" class="px-2 pt-4 border border-1 border-rounded rounded-3">
            <h6 class="text-uppercase">
                <i v-if="areAllprerequisitesMet" class="bi bi-check-circle-fill pe-1 text-success"></i>
                <i v-else class="bi bi-x-circle-fill pe-1 text-danger"></i>
                Prerequisites
            </h6>
            <ul>
                <li v-for="(item, index) in prerequisites" :key="index" :class="prerequisiteTextClass(index)">{{ item }}
                </li>
            </ul>
        </div>

        <div v-if="functionStatus === FunctionStatus.InProgress"
            class="d-flex justify-content-center align-items-center mt-4">
            <div class="spinner-border" aria-hidden="true"></div>
            <strong class="ms-2" role="status">{{ executionAborted ? "Interruzzione in corso..." : "In esecuzione..."
                }}</strong>
        </div>

        <div class="d-grid gap-2 mt-4">
            <button v-if="functionStatus === FunctionStatus.Done || functionStatus === FunctionStatus.Failed"
                class="btn btn-primary" :disabled="!areAllprerequisitesMet" @click="startFunction">
                Start Function
            </button>
            <button v-if="functionStatus === FunctionStatus.InProgress && !executionAborted" class="btn btn-warning"
                @click="stopFunction">
                Stop Function
            </button>
        </div>
    </div>
</template>

<script setup lang="ts">
import { computed, ref, onBeforeMount, onBeforeUnmount } from 'vue';
import api from '@/api/api';
import { useGameFunctionsStore } from '@/stores/gameFunctions'
import ErrorDialog from '@/components/dialogs/ErrorDialog.vue';
import type { GetFunctionStatusResponse, GetFunctionPrerequisitesResponse, GetFunctionStartResponse } from '@/api/functionsApi';
import { FunctionStatus } from '@/api/functionsApi';

const props = defineProps<{
    functionGroupName: string,
    webFunctionName: string,
}>();

const functionsStore = useGameFunctionsStore();
const functionsGroup = computed(() => functionsStore.groups.find((x) => x.name == props.functionGroupName));
const webFunction = computed(() => functionsGroup.value?.functions.find((x) => x.name == props.webFunctionName));

const prerequisites = computed(() => webFunction.value?.prerequisites && webFunction.value?.prerequisites.length > 0 ? webFunction.value?.prerequisites : []);

const loading = ref(false);
const apiError = ref<string | null>(null);

const functionStatus = ref<FunctionStatus>(FunctionStatus.Unknown);
const arePrerequisitesMet = ref<boolean[]>([]);
const areAllprerequisitesMet = ref(false);
const executionAborted = ref(false);

const showErrorDialog = ref(false);
const errorDialogMessage = ref('');

const emit = defineEmits<{
    (e: 'functionDone'): void;
    (e: 'functionFailed', error: string): void;
}>();

let pollingActive = true;

onBeforeMount(async () => {
    loading.value = true;
    startPollingStatus();
});

onBeforeUnmount(() => {
    pollingActive = false;
});

function prerequisiteTextClass(index: number) {
    return {
        'text-success': arePrerequisitesMet.value[index],
        'text-danger': !arePrerequisitesMet.value[index]
    };
}

async function startPollingStatus() {
    while (pollingActive) {
        await updateCurrentStatus();
        loading.value = false;
        await new Promise(resolve => setTimeout(resolve, 500));
    }
}

async function updateCurrentStatus() {
    try {
        const [newStatus, failureDescription] = await getFunctionStatus();
        if (functionStatus.value === FunctionStatus.InProgress && newStatus === FunctionStatus.Failed) {
            if (!executionAborted.value) {
                errorDialogMessage.value = failureDescription || 'Function execution failed';
                showErrorDialog.value = true;
                emit('functionFailed', errorDialogMessage.value);
            }            
        }
        if (functionStatus.value === FunctionStatus.InProgress && newStatus === FunctionStatus.Done) {
            if (!executionAborted.value) {
                emit('functionDone');
            }
        }
        functionStatus.value = newStatus;
    } catch (error) {
        functionStatus.value = FunctionStatus.Unknown;
        apiError.value = 'Failed to fetch function status: ' + error;
        return;
    }

    if (webFunction.value?.prerequisites && webFunction.value?.prerequisites.length > 0) {
        try {
            arePrerequisitesMet.value = await getPrerequisitesMet();
            areAllprerequisitesMet.value = arePrerequisitesMet.value.length > 0 && arePrerequisitesMet.value.every(Boolean);
        } catch (error) {
            arePrerequisitesMet.value = [];
            areAllprerequisitesMet.value = false;
            apiError.value = 'Failed to fetch prerequisites met status: ' + error;
            return;
        }
    } else {
        arePrerequisitesMet.value = [];
        areAllprerequisitesMet.value = true;
    }

    apiError.value = null;
}

async function getFunctionStatus(): Promise<[FunctionStatus, string | null]> {
    const groupName = functionsGroup.value?.name;
    const functionName = webFunction.value?.name;
    if (!groupName || !functionName)
        return [FunctionStatus.Unknown, null];
    try {
        const result = await api.get<GetFunctionStatusResponse>(`/webfunctions/${groupName}/${functionName}/status`)
        return [parseFunctionStatus(result.data.status as unknown as string), result.data.failure_description];
    } catch (error) {
        console.error('Error fetching function status:', error);
        throw error;
    }
}

function parseFunctionStatus(status: string): FunctionStatus {
    switch (status) {
        case FunctionStatus.Done: return FunctionStatus.Done;
        case FunctionStatus.InProgress: return FunctionStatus.InProgress;
        case FunctionStatus.Failed: return FunctionStatus.Failed;
        default: return FunctionStatus.Unknown;
    }
}

async function getPrerequisitesMet(): Promise<boolean[]> {
    const groupName = functionsGroup.value?.name;
    const functionName = webFunction.value?.name;
    if (!groupName || !functionName)
        return [];

    try {
        const result = await api.get<GetFunctionPrerequisitesResponse>(`/webfunctions/${groupName}/${functionName}/prerequisites`)
        return result.data.prerequisites_met;
    } catch (error) {
        console.error('Error fetching prerequisites status:', error);
        throw error;
    }
}

async function startFunction() {
    const groupName = functionsGroup.value?.name;
    const functionName = webFunction.value?.name;
    if (!groupName || !functionName || !areAllprerequisitesMet.value)
        return;

    try {
        executionAborted.value = false;
        const result = await api.get<GetFunctionStartResponse>(`/webfunctions/${groupName}/${functionName}/start`);
        result.data.status = parseFunctionStatus(result.data.status as unknown as string);

        functionStatus.value = result.data.status;
        if (result.data.status === FunctionStatus.Failed) {
            errorDialogMessage.value = result.data.failure_description || 'Function failed to start.';
            showErrorDialog.value = true;
        }
    } catch (error) {
        console.error('Error starting function:', error);
        errorDialogMessage.value = 'Failed to start function: ' + error;
        showErrorDialog.value = true;
    }
}

async function stopFunction() {
    const groupName = functionsGroup.value?.name;
    const functionName = webFunction.value?.name;
    if (!groupName || !functionName)
        return;

    try {
        executionAborted.value = true;
        await api.get(`/webfunctions/${groupName}/${functionName}/stop`);
    } catch (error) {
        console.error('Error stopping function:', error);
        errorDialogMessage.value = 'Failed to stop function: ' + error;
        showErrorDialog.value = true;
    }
}

</script>
