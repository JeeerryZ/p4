import { useState } from 'react';
import { useEvaluationStore } from '../store/evaluationStore';

export function RawOutput() {
  const response = useEvaluationStore((s) => s.response);
  const [open, setOpen] = useState(false);

  if (!response?.raw_output) return null;

  return (
    <div className="animate-fade-in border border-p4-border rounded overflow-hidden">
      <button
        onClick={() => setOpen((v) => !v)}
        className="
          w-full flex items-center justify-between
          px-4 py-3 bg-p4-elevated text-left
          hover:bg-p4-border/30 transition-colors duration-150
          focus:outline-none
        "
      >
        <span className="flex items-center gap-2 text-sm text-p4-muted font-medium">
          <TerminalIcon />
          Maple output
        </span>
        <ChevronIcon open={open} />
      </button>
      {open && (
        <pre className="border-t border-p4-border bg-p4-surface px-4 py-3 text-xs font-mono text-p4-muted overflow-x-auto leading-relaxed whitespace-pre-wrap max-h-64 overflow-y-auto">
          {response.raw_output.trim() || '(empty)'}
        </pre>
      )}
    </div>
  );
}

function TerminalIcon() {
  return (
    <svg className="w-3.5 h-3.5" viewBox="0 0 20 20" fill="currentColor">
      <path
        fillRule="evenodd"
        d="M2 5a2 2 0 012-2h12a2 2 0 012 2v10a2 2 0 01-2 2H4a2 2 0 01-2-2V5zm3.293 1.293a1 1 0 011.414 0l3 3a1 1 0 010 1.414l-3 3a1 1 0 01-1.414-1.414L7.586 10 5.293 7.707a1 1 0 010-1.414zM11 12a1 1 0 100 2h3a1 1 0 100-2h-3z"
        clipRule="evenodd"
      />
    </svg>
  );
}

function ChevronIcon({ open }: { open: boolean }) {
  return (
    <svg
      className={`w-4 h-4 text-p4-muted transition-transform duration-200 ${open ? 'rotate-180' : ''}`}
      viewBox="0 0 20 20"
      fill="currentColor"
    >
      <path
        fillRule="evenodd"
        d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z"
        clipRule="evenodd"
      />
    </svg>
  );
}
