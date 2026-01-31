import axios from "axios";
const apiBaseUrl = import.meta.env.VITE_API_BASE_URL || window.location.origin;

const api = axios.create({
    baseURL: apiBaseUrl,
    timeout: 2000,
    headers: { 'Content-Type': 'application/json'}
});

export default api;