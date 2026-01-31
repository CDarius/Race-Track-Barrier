<template>
    <GenericDialog title="Attenzione" titleClass="text-bg-warning" title-icon="bi-exclamation-triangle-fill"
        v-model:is-open="showConfirmSaveDialog" :buttons="[
            {
                text: 'Annulla', class: 'btn-secondary', callback: () => {
                    goBack();
                }
            },
            {
                text: 'Continua', class: 'btn-warning', callback: () => {
                    saveBackup();
                }
            }
        ]">
        La lettura di {{ numberFailedReadGameSettings }} impostazioni su {{ totalNumberOfSettings }} è fallita.<br />
        Sei sicuro di voler continuare?
    </GenericDialog>

    <div v-if="settingStore.groupsLoading"
        class="h-100 w-100 d-flex flex-column justify-content-center align-items-center">
        <h3>Loading...</h3>
        <div class="spinner-border mt-2" style="width: 3rem; height: 3rem;" role="status">
            <span class="visually-hidden">Loading...</span>
        </div>
    </div>

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
            <h1>Settings backup</h1>
        </div>

        <div v-if="backupInProgress" class="d-grid col-12 col-md-6 mx-auto px-4">
            <!-- Current settting -->
            <div><small class="text-body-secondary">{{ currentSettingPath }}</small></div>
            <!-- Progress bar -->
            <div class="progress" role="progressbar" aria-label="Animated striped example" aria-valuenow="75"
                aria-valuemin="0" aria-valuemax="100">
                <div class="progress-bar progress-bar-striped progress-bar-animated" :style="{ width: progress + '%' }">
                    {{ Math.round(progress) }}%
                </div>
            </div>
        </div>
        <div v-else class="d-grid col-12 col-md-6 mx-auto px-4">
            <h5>Backup completed</h5>
            <div>Successfully read: <span class="text-success">{{ numberSuccessfulReadGameSettings }}</span> settings
            </div>
            <div v-if="numberFailedReadGameSettings > 0">
                Failed read: <span class="text-danger">{{ numberFailedReadGameSettings }}</span> settings
            </div>
            <button class="btn btn-primary mt-3" @click="goBack">Go back</button>
        </div>
    </div>
</template>

<script lang="ts" setup>
import { onMounted, ref } from 'vue';
import { useRouter } from 'vue-router';
import { useGameSettingsStore } from '@/stores/gameSettings'
import GenericDialog from '@/components/dialogs/GenericDialog.vue';

const settingStore = useGameSettingsStore();
const router = useRouter();

const backupInProgress = ref(true);
const currentSettingPath = ref('');
const progress = ref(0);

const showConfirmSaveDialog = ref(false);
let numberFailedReadGameSettings = 0;
let numberSuccessfulReadGameSettings = 0;
let totalNumberOfSettings = 0
let backupData: { path: string, value: unknown }[] = [];

onMounted(async () => {
    // Fetch game settings list if not already available
    if (settingStore.groups == null || settingStore.groups.length == 0)
        await settingStore.fetchGameSettingsList();

    if (settingStore.groupsLoadingError)
        return;

    // Read all settings values
    ({ success: numberSuccessfulReadGameSettings, failed: numberFailedReadGameSettings, backup: backupData } = await runBackup());
    totalNumberOfSettings = numberSuccessfulReadGameSettings + numberFailedReadGameSettings;

    // Show backup resuls
    backupInProgress.value = false;

    // If there are failed settings, show the confirmation dialog
    if (numberFailedReadGameSettings > 0) {
        showConfirmSaveDialog.value = true;
        return;
    }

    saveBackup();
});

async function runBackup() {
    const backup: { path: string, value: unknown }[] = [];

    // Assuming you have access to your store instance as settingStore
    const totalSettings = settingStore.groups.reduce(
        (sum, group) => sum + group.settings.length,
        0
    );

    let numCompletedSettings = 0;
    let numSuccessSettings = 0;
    let numFailedSettings = 0;

    for (const group of settingStore.groups) {
        for (const setting of group.settings) {
            const apiPath = `/settings/${group.name}/${setting.name}`;
            currentSettingPath.value = apiPath;

            try {
                const response = await settingStore.readGameSettingValue(group.name, setting.name);
                backup.push({ path: apiPath, value: response.value });
                numSuccessSettings++;
            } catch (error) {
                console.error(`Error reading setting value at "${apiPath}": `, error);
                numFailedSettings++;
            } finally {
                numCompletedSettings++;
                progress.value = (numCompletedSettings / totalSettings) * 100;
            }

        }
    }
    return { backup: backup, success: numSuccessSettings, failed: numFailedSettings };
}

// Prompt the user to save the backup
function saveBackup() {
    // Create a JSON blob from backupData
    const json = JSON.stringify(backupData, null, 2);
    const blob = new Blob([json], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `pong-settings-backup-${new Date().toISOString().slice(0, 19).replace(/[:T]/g, '-')}.pong`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
}

function goBack() {
    router.back();
}

</script>