import { useState } from 'react';
import type { Singularity, SingularityType } from '../lib/types';
import { useEvaluationStore } from '../store/evaluationStore';

type Tab = 'finite' | 'infinite';

const TYPE_STYLES: Record<SingularityType, string> = {
  saddle:          'bg-orange-500/15 text-orange-300 border border-orange-500/25',
  node:            'bg-green-500/15 text-green-300 border border-green-500/25',
  strong_focus:    'bg-blue-500/15 text-blue-300 border border-blue-500/25',
  weak_focus:      'bg-cyan-500/15 text-cyan-300 border border-cyan-500/25',
  semi_elementary: 'bg-purple-500/15 text-purple-300 border border-purple-500/25',
  degenerate:      'bg-red-500/15 text-red-300 border border-red-500/25',
};

const TYPE_LABELS: Record<SingularityType, string> = {
  saddle:          'Saddle',
  node:            'Node',
  strong_focus:    'Strong Focus',
  weak_focus:      'Weak Focus',
  semi_elementary: 'Semi-Elementary',
  degenerate:      'Degenerate',
};

function stabilityLabel(s: Singularity): string {
  switch (s.type) {
    case 'node':
    case 'strong_focus':
      if (s.stable === -1) return 'stable';
      if (s.stable === 1) return 'unstable';
      if (s.stable === -2) return 'center-stable';
      if (s.stable === 2) return 'center-unstable';
      return '—';
    case 'weak_focus':
      if (s.wtype === 0) return 'center';
      if (s.wtype === -1) return 'stable';
      if (s.wtype === 1) return 'unstable';
      return `wtype ${s.wtype ?? '?'}`;
    case 'semi_elementary':
      return `se-type ${s.setype ?? '?'}`;
    case 'saddle':
    case 'degenerate':
      return s.notadummy === false ? 'dummy' : '—';
    default:
      return '—';
  }
}

function fmtCoord(n: number): string {
  if (Number.isInteger(n)) return n.toFixed(0);
  const s = n.toPrecision(6);
  return parseFloat(s).toString();
}

function SingularityRow({ s }: { s: Singularity }) {
  return (
    <tr className="border-t border-p4-border hover:bg-p4-elevated/50 transition-colors duration-100">
      <td className="px-4 py-2.5">
        <span className={`inline-block px-2 py-0.5 rounded text-xs font-medium ${TYPE_STYLES[s.type]}`}>
          {TYPE_LABELS[s.type]}
        </span>
      </td>
      <td className="px-4 py-2.5 font-mono text-xs text-p4-text tabular-nums">
        ({fmtCoord(s.x0)}, {fmtCoord(s.y0)})
      </td>
      <td className="px-4 py-2.5 text-xs text-p4-muted font-mono">
        {s.chart}
      </td>
      <td className="px-4 py-2.5 text-xs text-p4-muted">
        {stabilityLabel(s)}
      </td>
    </tr>
  );
}

function EmptyState({ label }: { label: string }) {
  return (
    <tr>
      <td colSpan={4} className="px-4 py-8 text-center text-p4-muted text-sm">
        No {label} singularities found.
      </td>
    </tr>
  );
}

export function SingularityTable() {
  const response = useEvaluationStore((s) => s.response);
  const [tab, setTab] = useState<Tab>('finite');

  if (!response) return null;

  const rows = tab === 'finite' ? response.finite_singularities : response.infinite_singularities;

  const tabClass = (t: Tab) =>
    `px-4 py-2 text-sm font-medium rounded-t transition-colors duration-150 focus:outline-none ${
      tab === t
        ? 'text-p4-text border-b-2 border-p4-amber'
        : 'text-p4-muted hover:text-p4-text'
    }`;

  return (
    <div className="animate-fade-in">
      {/* Tab bar */}
      <div className="flex items-center gap-1 border-b border-p4-border mb-0">
        <button className={tabClass('finite')} onClick={() => setTab('finite')}>
          Finite
          <span className="ml-1.5 text-xs opacity-60">
            ({response.finite_singularities.length})
          </span>
        </button>
        <button className={tabClass('infinite')} onClick={() => setTab('infinite')}>
          Infinite
          <span className="ml-1.5 text-xs opacity-60">
            ({response.infinite_singularities.length})
          </span>
        </button>
      </div>

      {/* Table */}
      <div className="overflow-x-auto rounded-b border border-t-0 border-p4-border bg-p4-surface">
        <table className="w-full text-sm">
          <thead>
            <tr className="border-b border-p4-border">
              <th className="px-4 py-2.5 text-left text-xs font-medium text-p4-muted uppercase tracking-wider">
                Type
              </th>
              <th className="px-4 py-2.5 text-left text-xs font-medium text-p4-muted uppercase tracking-wider">
                Position
              </th>
              <th className="px-4 py-2.5 text-left text-xs font-medium text-p4-muted uppercase tracking-wider">
                Chart
              </th>
              <th className="px-4 py-2.5 text-left text-xs font-medium text-p4-muted uppercase tracking-wider">
                Stability
              </th>
            </tr>
          </thead>
          <tbody>
            {rows.length === 0
              ? <EmptyState label={tab} />
              : rows.map((s, i) => <SingularityRow key={i} s={s} />)
            }
          </tbody>
        </table>
      </div>

      {/* Summary line */}
      <p className="mt-2 text-xs text-p4-muted">
        p = {response.p}, q = {response.q}
        {response.singinf && (
          <span className="ml-3 text-p4-amber/70">∞ singularities present</span>
        )}
      </p>
    </div>
  );
}
