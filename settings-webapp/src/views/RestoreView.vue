<template>
    <error-dialog v-model:is-open="showErrorDialog" title="Errror" :message="errorMessage" />

    <SpinnerBackdrop v-if="settingStore.groupsLoading" text="Loading..." />
    <SpinnerBackdrop v-if="status === 'saving'" text="Saving..." />

    <div v-else-if="settingStore.groupsLoadingError" class="container-md">
        <div class="alert alert-danger d-flex align-items-center mt-4" role="alert">
            <i class="bi bi-exclamation-triangle-fill me-2"></i>
            <div>
                {{ settingStore.groupsLoadingError }}
            </div>
        </div>
    </div>

    <div v-else>
        <div class="py-2 ps-2 mb-5 bg-light">
            <h1>Restore backup</h1>
        </div>
        <div v-if="status === 'selectFile'" class="d-grid col-12 col-md-6 mx-auto px-4">
            <label for="restore-file" class="form-label">Select backup file:</label>
            <input type="file" accept=".pong" id="restore-file" class="form-control" @change="onFileChange" />
            <button class="btn btn-primary mt-3" @click="restoreSettings" :disabled="!selectedFile">Restore</button>
        </div>
        <div v-else-if="status === 'restoring'" class="d-grid col-12 col-md-6 mx-auto px-4">
            <!-- Current settting -->
            <div><small class="text-body-secondary">{{ currentSettingPath }}</small></div>
            <!-- Progress bar -->
            <div class="progress" role="progressbar" aria-label="Animated striped example" aria-valuenow="75"
                aria-valuemin="0" aria-valuemax="100">
                <div class="progress-bar progress-bar-striped progress-bar-animated" :style="{ width: progress + '%' }">
                    {{ Math.round(progress) }}%
                </div>
            </div>
            <button class="btn btn-danger mt-2" @click="restoreAborted = true">
                <i class="bi bi-x-octagon me-2"></i>
                Abort
            </button>
        </div>
        <div v-else-if="status === 'results' || status === 'saving'" class="d-grid col-12 col-md-6 mx-auto px-4">
            <h5>Restore completed</h5>
            <div>Successfully restored: <span class="text-success">{{ numRestoreSuccess }}</span> settings</div>
            <div v-if="numMissingSettings > 0">
                Missing settings (not in backup): <span class="text-warning">{{ numMissingSettings }}</span> settings
            </div>
            <div v-if="numRestoreErrors > 0">
                Errors during restore: <span class="text-danger">{{ numRestoreErrors }}</span> settings
            </div>
            <button class="btn btn-warning mt-3" @click="saveSettings" :disabled="numMissingSettings > 0">
                Save settings
            </button>
            <button class="btn btn-secondary mt-3 ms-2" @click="goBack()">Go back</button>
        </div>
        <div v-if="status != 'selectFile' && numRestoreErrors > 0" class="d-grid col-12 col-md-6 mx-auto px-4 mt-3">
            <h5 class="text-danger">Errors</h5>
            <ul>
                <li v-for="(error, index) in restoreErrors" :key="index">{{ error }}</li>
            </ul>
        </div>
    </div>
</template>

<script lang="ts" setup>
import { onMounted, ref, computed } from 'vue';
import { useRouter } from 'vue-router';
import { useGameSettingsStore } from '@/stores/gameSettings'
import ErrorDialog from '@/components/dialogs/ErrorDialog.vue';

const settingStore = useGameSettingsStore();
const router = useRouter();


const status = ref<'selectFile' | 'restoring' | 'saving' | 'results'>('selectFile');
const currentSettingPath = ref('');
const progress = ref(0);
const selectedFile = ref<File | null>(null);
let restoreAborted = false;

const showErrorDialog = ref(false)
const errorMessage = ref('');

const settingApiPaths: string[] = [];
const restoreErrors = ref<string[]>([]);
const numRestoreErrors = computed(() => restoreErrors.value.length);
const numRestoreSuccess = ref(0);
const numMissingSettings = ref(0);

function onFileChange(event: Event) {
    const input = event.target as HTMLInputElement;
    selectedFile.value = input.files && input.files.length > 0 ? input.files[0] : null;
}

onMounted(async () => {
    // Fetch game settings list if not already available
    if (settingStore.groups == null || settingStore.groups.length == 0)
        await settingStore.fetchGameSettingsList();

    if (settingStore.groupsLoadingError)
        return;

    // Create a list of API paths for all settings
    for (const group of settingStore.groups) {
        for (const setting of group.settings) {
            const apiPath = `/settings/${group.name}/${setting.name}`;
            settingApiPaths.push(apiPath);
        }
    }
})

type RestoreSetting = {
    path: string;
    value: unknown;
};

async function restoreSettings() {
    if (!selectedFile.value) return;

    restoreErrors.value = [];
    status.value = 'restoring';
    restoreAborted = false;
    progress.value = 0;
    currentSettingPath.value = '';

    // Read and parse the backup file
    let json: RestoreSetting[] = [];
    try {
        const file = selectedFile.value;
        const text = await file.text();
        json = JSON.parse(text);
    } catch (err) {
        errorMessage.value = 'Failed to read or parse file:' + err;
        showErrorDialog.value = true;
        status.value = 'selectFile';
        return;
    }

    // Restore settings one by one
    for (const [index, item] of json.entries()) {
        if (restoreAborted) {
            status.value = 'selectFile';
            return;
        }

        // Update UI with current progress
        currentSettingPath.value = item.path;
        progress.value = (index + 1) / json.length * 100;

        // Test API exist
        if (!settingApiPaths.includes(item.path)) {
            restoreErrors.value.push(`Setting no longer exists: ${item.path}`);
            return;
        }

        // Restore setting value
        await restoreSetting(item);
    }

    // Calculate summary statistics
    numRestoreSuccess.value = json.length - restoreErrors.value.length;
    numMissingSettings.value = settingApiPaths.length - numRestoreSuccess.value;

    if (numMissingSettings.value == 0) {
        saveSettings();
    } else {
        // Show results and prompt to save
        status.value = 'results';
    }
}

async function restoreSetting(setting: RestoreSetting) {
    const { group, name } = extractGroupAndName(setting.path) || {};
    if (!group || !name) {
        restoreErrors.value.push(`Invalid setting path: ${setting.path}`);
        return;
    }

    try {
        await settingStore.writeGameSettingValue(group, name, setting.value);
    } catch (err) {
        restoreErrors.value.push(`Failed to restore setting ${setting.path}: ${err}`);
    }
}

function extractGroupAndName(path: string): { group: string; name: string } | null {
    const match = path.match(/^\/settings\/([^\/]+)\/([^\/]+)$/);
    if (!match) return null; // Format does not match exactly
    const [, group, name] = match;
    return { group, name };
}

async function saveSettings() {
    status.value = 'saving';
    try {
        await settingStore.saveSettingsToNVS();
    } catch (err) {
        errorMessage.value = 'Failed to save settings:' + err;
        showErrorDialog.value = true;
    } finally {
        status.value = 'results';
    }
}

function goBack() {
    router.back();
}
</script>