import { create } from 'zustand';
import type { EvaluateRequest, EvaluateResponse } from '../lib/types';

export type EvalStatus = 'idle' | 'loading' | 'success' | 'error';

interface EvaluationState {
  xDot: string;
  yDot: string;
  response: EvaluateResponse | null;
  status: EvalStatus;
  error: string | null;
  mapleOutput: string | null;

  setXDot: (v: string) => void;
  setYDot: (v: string) => void;
  setLoading: () => void;
  setSuccess: (r: EvaluateResponse) => void;
  setError: (msg: string, mapleOutput?: string) => void;
  buildRequest: () => EvaluateRequest;
}

export const useEvaluationStore = create<EvaluationState>((set, get) => ({
  xDot: '',
  yDot: '',
  response: null,
  status: 'idle',
  error: null,
  mapleOutput: null,

  setXDot: (v) => set({ xDot: v }),
  setYDot: (v) => set({ yDot: v }),

  setLoading: () => set({ status: 'loading', error: null, mapleOutput: null }),

  setSuccess: (r) => set({ status: 'success', response: r }),

  setError: (msg, mapleOutput) =>
    set({ status: 'error', error: msg, mapleOutput: mapleOutput ?? null }),

  buildRequest: (): EvaluateRequest => ({
    x_dot: get().xDot.trim(),
    y_dot: get().yDot.trim(),
  }),
}));
