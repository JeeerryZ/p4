import type { EvaluateRequest, EvaluateResponse, EvaluateError } from './types';

export class ApiError extends Error {
  status: number;
  mapleOutput?: string;

  constructor(message: string, status: number, mapleOutput?: string) {
    super(message);
    this.name = 'ApiError';
    this.status = status;
    this.mapleOutput = mapleOutput;
  }
}

export async function evaluate(req: EvaluateRequest): Promise<EvaluateResponse> {
  const controller = new AbortController();
  // 65 s timeout — Maple's own time_limit is 60 s, so this gives it a margin
  const timer = setTimeout(() => controller.abort(), 65_000);

  try {
    const res = await fetch('/api/evaluate', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(req),
      signal: controller.signal,
    });

    const body = await res.json();

    if (!res.ok) {
      const err = body as EvaluateError;
      throw new ApiError(
        err.error ?? `HTTP ${res.status}`,
        res.status,
        err.maple_output,
      );
    }

    return body as EvaluateResponse;
  } catch (e) {
    if (e instanceof ApiError) throw e;
    if (e instanceof DOMException && e.name === 'AbortError') {
      throw new ApiError('Request timed out after 65 seconds', 408);
    }
    throw new ApiError(
      e instanceof Error ? e.message : 'Network error',
      0,
    );
  } finally {
    clearTimeout(timer);
  }
}
