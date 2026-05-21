import { EquationForm } from './components/EquationForm';
import { SingularityTable } from './components/SingularityTable';
import { RawOutput } from './components/RawOutput';
import { ErrorBanner } from './components/ErrorBanner';
import { useEvaluationStore } from './store/evaluationStore';

export default function App() {
  const status = useEvaluationStore((s) => s.status);
  const hasResults = status === 'success';
  const hasError = status === 'error';

  return (
    <div className="min-h-screen bg-p4-bg font-sans">
      <div className="max-w-3xl mx-auto px-6 py-12">

        {/* ── Header ────────────────────────────────────────────────── */}
        <header className="mb-10">
          <div className="flex items-baseline gap-4 mb-1">
            <h1 className="font-display text-5xl font-light tracking-tight text-p4-text leading-none">
              P4
            </h1>
            <span className="font-display italic text-p4-muted text-base font-light">
              phase portrait analyser
            </span>
          </div>
          <p className="text-p4-muted text-xs font-mono mt-1">
            Polynomial Planar Phase Portraits — Poincaré compactification
          </p>
          <div className="mt-5 h-px bg-gradient-to-r from-p4-amber/40 via-p4-border to-transparent" />
        </header>

        {/* ── Equation input form ───────────────────────────────────── */}
        <section className="mb-8">
          <EquationForm />
        </section>

        {/* ── Results ─────────────────────────────────────────────── */}
        {(hasResults || hasError) && (
          <section className="space-y-5">
            <div className="h-px bg-p4-border" />

            {hasError && <ErrorBanner />}

            {hasResults && (
              <>
                <SingularityTable />
                <RawOutput />
              </>
            )}
          </section>
        )}

        {/* ── Footer ───────────────────────────────────────────────── */}
        <footer className="mt-16 pt-6 border-t border-p4-border flex items-center justify-between">
          <span className="text-p4-muted/50 text-xs font-mono">
            P4 web — LGPL
          </span>
          <span className="text-p4-muted/30 text-xs">
            powered by Maple
          </span>
        </footer>

      </div>
    </div>
  );
}
