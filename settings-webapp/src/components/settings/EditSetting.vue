<script setup lang="ts">
import { ref, onBeforeMount, computed } from 'vue';
import axios from 'axios';
import type { CancelTokenSource } from 'axios';
import { debounce } from '@/utils/debounce';
import type { GameSettingItem } from '@/api/settingsApi';
import NumericValueEditor from '../NumericValueEditor.vue';
import { useGameSettingsStore } from '@/stores/gameSettings'
import BooleanValueEditor from '../BooleanValueEditor.vue';

const props = withDefaults(defineProps<{
    groupName: string,
    setting: GameSettingItem,
    valueOnly?: boolean,
    showUnit?: boolean
}>(), {
    valueOnly: false,
    showUnit: true
});


const settingStore = useGameSettingsStore();

const loadingValue = ref(true);
const loadingValueFailed = ref(false);
const settingValue = ref<number|boolean>(0);
const updateInProgress = ref(false);

const showValueEditor = computed(() => {
    return !loadingValue.value && !loadingValueFailed.value;
});

const numberSettingValue = computed(() => {
    return typeof settingValue.value === 'number' ? settingValue.value : 0;
});

const booleanSettingValue = computed(() => {
    return typeof settingValue.value === 'boolean' ? settingValue.value : false;
});

onBeforeMount(() => {
    fetchSettingValue();
});

async function fetchSettingValue() {
    loadingValue.value = true;
    loadingValueFailed.value = false;

    try {
        const response = await settingStore.readGameSettingValue(props.groupName, props.setting.name);
        settingValue.value = response.value;
    } catch (error) {
        loadingValueFailed.value = true;
        console.error('Error fetching setting value:', error);
    } finally {
        loadingValue.value = false;
    }
}

// Expose the function to parent components
defineExpose({
    fetchSettingValue
});

let cancelTokenSource: CancelTokenSource | null = null;

const debouncedUpdateValue = debounce(async (value: number|boolean) => {
    // Cancel previous request
    if (cancelTokenSource) {
        cancelTokenSource.cancel('Previous request canceled');
    }

    cancelTokenSource = axios.CancelToken.source();

    try {
        const response = await settingStore.writeGameSettingValue(props.groupName, props.setting.name, value, cancelTokenSource);
        settingValue.value = response.data.value;
    } catch (error) {
        if (!axios.isCancel(error)) {
            settingValue.value = settingValue.value;
            console.error('Error updating setting value:', error);
        }
    } finally {
        cancelTokenSource = null;
        updateInProgress.value = false;
    }
}, 1000);

function updateSettingValue(value: number|boolean) {
    if (typeof value === 'number') {
        if (props.setting.minValue != null && value < props.setting.minValue) {
            value = props.setting.minValue;
        }
        if (props.setting.maxValue != null && value > props.setting.maxValue) {
            value = props.setting.maxValue;
        }
    }
    if (value !== settingValue.value) {
        updateInProgress.value = true;
        debouncedUpdateValue(value);
    }
}

</script>

<template>
    <div v-if="!props.valueOnly" class="card">
        <div class="card-body">
            <h5 class="card-title">{{ setting.title }}</h5>
            <p class="card-text">{{ setting.description }}</p>
        </div>
        <div class="card-footer">
            <div v-if="loadingValue" class="spinner-border float-end" role="status">
                <span class="visually-hidden">Loading...</span>
            </div>
            <div v-if="loadingValueFailed" class="float-end text-danger">
                <i class="bi bi-exclamation-triangle"></i>
                Value read failed
            </div>
            <div v-if="showValueEditor" class="d-flex justify-content-end align-items-center">
                <div v-if="props.showUnit" class="me-2">{{ setting.unit }}</div>
                <BooleanValueEditor v-if="setting.type == 'bool'" :value="booleanSettingValue" @updateValue="updateSettingValue"
                    :update-in-progress="updateInProgress">
                </BooleanValueEditor>
                <NumericValueEditor v-else :value="numberSettingValue" :step-change="setting.stepChange"
                    :min-value="setting.minValue" class="float-end" :max-value="setting.maxValue"
                    :update-in-progress="updateInProgress" @updateValue="updateSettingValue">
                </NumericValueEditor>
            </div>
        </div>
    </div>
    <div v-else>
        <div v-if="loadingValue" class="spinner-border float-end" role="status">
            <span class="visually-hidden">Loading...</span>
        </div>
        <div v-if="loadingValueFailed" class="float-end text-danger">
            <i class="bi bi-exclamation-triangle"></i>
            Value read failed
        </div>
        <div v-if="showValueEditor" class="d-flex align-items-center">
            <div v-if="props.showUnit" class="me-2">{{ setting.unit }}</div>
            <BooleanValueEditor v-if="setting.type == 'bool'" :value="booleanSettingValue" @updateValue="updateSettingValue"
                :update-in-progress="updateInProgress">
            </BooleanValueEditor>
            <NumericValueEditor v-else :value="numberSettingValue" :step-change="setting.stepChange"
                :min-value="setting.minValue" :max-value="setting.maxValue"
                :update-in-progress="updateInProgress" @updateValue="updateSettingValue">
            </NumericValueEditor>
        </div>
    </div>

</template>