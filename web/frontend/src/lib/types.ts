export interface PolyTerm {
  coeff: number;
  exp_x: number;
  exp_y: number;
}

export type SingularityType =
  | 'saddle'
  | 'node'
  | 'strong_focus'
  | 'weak_focus'
  | 'semi_elementary'
  | 'degenerate';

export type ChartType = 'R2' | 'U1' | 'U2' | 'V1' | 'V2';

export interface Singularity {
  type: SingularityType;
  x0: number;
  y0: number;
  chart: ChartType;
  notadummy?: boolean;
  stable?: number;   // -1=stable, 1=unstable, -2=center-stable, 2=center-unstable
  wtype?: number;    // weak focus: 0=center, -1=stable, 1=unstable
  setype?: number;   // semi-elementary type index
}

export interface EvaluateRequest {
  x_dot: string;
  y_dot: string;
  gcf?: string;
  precision?: number;
  epsilon?: number;
  taylor_level?: number;
  numeric_level?: number;
  max_level?: number;
  weakness_level?: number;
  p?: number;
  q?: number;
  numeric?: boolean;
  test_sep?: boolean;
  time_limit?: number;
}

export interface EvaluateResponse {
  finite_singularities: Singularity[];
  infinite_singularities: Singularity[];
  vec_field: [PolyTerm[], PolyTerm[]];
  p: number;
  q: number;
  typeofstudy: number;
  singinf: boolean;
  raw_output: string;
}

export interface EvaluateError {
  error: string;
  maple_output?: string;
}
