import { useEvaluationStore } from '../store/evaluationStore';
import { evaluate, ApiError } from '../lib/api';

export function EquationForm() {
  const { xDot, yDot, status, setXDot, setYDot, setLoading, setSuccess, setError, buildRequest } =
    useEvaluationStore();

  const loading = status === 'loading';
  const canSubmit = xDot.trim().length > 0 && yDot.trim().length > 0 && !loading;

  async function handleSubmit(e: React.FormEvent) {
    e.preventDefault();
    if (!canSubmit) return;
    setLoading();
    try {
      const result = await evaluate(buildRequest());
      setSuccess(result);
    } catch (err) {
      if (err instanceof ApiError) {
        setError(err.message, err.mapleOutput);
      } else {
        setError('Unexpected error occurred');
      }
    }
  }

  return (
    <form onSubmit={handleSubmit} className="space-y-5">
      {/* x' equation */}
      <div className="flex items-start gap-4">
        <label className="flex-shrink-0 pt-3 w-24 text-right font-mono text-p4-amber text-sm select-none">
          dx/dt =
        </label>
        <div className="flex-1">
          <textarea
            className="equation-input"
            rows={2}
            value={xDot}
            onChange={(e) => setXDot(e.target.value)}
            placeholder="e.g.  x - y^2"
            disabled={loading}
            spellCheck={false}
            autoComplete="off"
          />
        </div>
      </div>

      {/* y' equation */}
      <div className="flex items-start gap-4">
        <label className="flex-shrink-0 pt-3 w-24 text-right font-mono text-p4-amber text-sm select-none">
          dy/dt =
        </label>
        <div className="flex-1">
          <textarea
            className="equation-input"
            rows={2}
            value={yDot}
            onChange={(e) => setYDot(e.target.value)}
            placeholder="e.g.  x^2 + y"
            disabled={loading}
            spellCheck={false}
            autoComplete="off"
          />
        </div>
      </div>

      {/* Submit row */}
      <div className="flex items-center justify-end gap-4 pt-1">
        {loading && (
          <span className="flex items-center gap-2 text-p4-muted text-sm">
            <SpinnerIcon />
            Maple evaluating…
          </span>
        )}
        <button
          type="submit"
          disabled={!canSubmit}
          className="
            px-6 py-2 rounded font-sans font-medium text-sm
            bg-p4-amber text-p4-bg
            hover:bg-p4-amber-dim
            disabled:opacity-40 disabled:cursor-not-allowed
            transition-colors duration-150
            focus:outline-none focus:ring-2 focus:ring-p4-amber focus:ring-offset-2 focus:ring-offset-p4-bg
          "
        >
          {loading ? 'Evaluating…' : 'Evaluate'}
        </button>
      </div>
    </form>
  );
}

function SpinnerIcon() {
  return (
    <svg
      className="animate-spin w-4 h-4 text-p4-amber"
      xmlns="http://www.w3.org/2000/svg"
      fill="none"
      viewBox="0 0 24 24"
    >
      <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="3" />
      <path
        className="opacity-75"
        fill="currentColor"
        d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z"
      />
    </svg>
  );
}
