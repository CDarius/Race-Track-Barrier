import axios  from 'axios';
import type { CancelTokenSource } from 'axios';
import { debounce } from './debounce.ts';

let cancelTokenSource: CancelTokenSource | null = null;

export const fetchData = async <T>(
  url: string,
  onSuccess: (data: T) => void,
  onError: (err: any) => void
) => {
  // Cancel previous request
  if (cancelTokenSource) {
    cancelTokenSource.cancel('Previous request canceled');
  }

  cancelTokenSource = axios.CancelToken.source();

  try {
    const response = await axios.get(url, {
      cancelToken: cancelTokenSource.token,
    });
    onSuccess(response.data);
  } catch (err: any) {
    if (!axios.isCancel(err)) {
      onError(err);
    }
  }
};