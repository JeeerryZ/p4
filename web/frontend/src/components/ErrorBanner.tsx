import { useState } from 'react';
import { useEvaluationStore } from '../store/evaluationStore';

export function ErrorBanner() {
  const { error, mapleOutput } = useEvaluationStore();
  const [showMaple, setShowMaple] = useState(false);

  if (!error) return null;

  return (
    <div className="animate-fade-in rounded border border-red-500/30 bg-red-500/10 overflow-hidden">
      <div className="flex items-start gap-3 px-4 py-3">
        <span className="text-red-400 mt-0.5 flex-shrink-0">
          <ErrorIcon />
        </span>
        <div className="flex-1 min-w-0">
          <p className="text-red-300 text-sm font-medium">{error}</p>
          {mapleOutput && (
            <button
              onClick={() => setShowMaple((v) => !v)}
              className="mt-1 text-xs text-red-400/70 hover:text-red-300 transition-colors underline decoration-dotted"
            >
              {showMaple ? 'Hide' : 'Show'} Maple output
            </button>
          )}
        </div>
      </div>
      {showMaple && mapleOutput && (
        <pre className="border-t border-red-500/20 bg-p4-surface px-4 py-3 text-xs font-mono text-p4-muted overflow-x-auto leading-relaxed whitespace-pre-wrap">
          {mapleOutput}
        </pre>
      )}
    </div>
  );
}

function ErrorIcon() {
  return (
    <svg className="w-4 h-4" viewBox="0 0 20 20" fill="currentColor">
      <path
        fillRule="evenodd"
        d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7 4a1 1 0 11-2 0 1 1 0 012 0zm-1-9a1 1 0 00-1 1v4a1 1 0 102 0V6a1 1 0 00-1-1z"
        clipRule="evenodd"
      />
    </svg>
  );
}
