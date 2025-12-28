# Shortest Path with Resource Constraints in DAG

A clean, well-documented implementation and experiments for finding shortest paths under resource constraints in Directed Acyclic Graphs (DAGs). This project implements exact and practical dynamic programming / label-setting techniques that respect an additive resource budget (e.g., time, cost, energy) while minimizing path cost.

Table of Contents
- [Problem Overview](#problem-overview)
- [Why DAGs?](#why-dags)
- [Features](#features)
- [Algorithm & Complexity](#algorithm--complexity)
- [Input / Output Formats](#input--output-formats)
- [Quickstart](#quickstart)
- [Examples](#examples)
- [Benchmarks & Experiments](#benchmarks--experiments)
- [Testing](#testing)
- [Development & Contribution](#development--contribution)
- [License](#license)
- [Citing](#citing)
- [Contact](#contact)

Problem Overview
----------------
Given a DAG G = (V, E), each edge e in E has:
- cost(e) — the value to minimize (e.g., distance, latency)
- resource(e) — the additive resource consumption (e.g., time, fuel)

Given a source s, target t, and a resource budget B, the goal is to find an s→t path P that:
- minimizes sum(cost(e) for e in P)
- subject to sum(resource(e) for e in P) ≤ B

This is the Constrained Shortest Path problem specialized to DAGs, where topological ordering enables efficient dynamic programming.

Why DAGs?
---------
- DAGs avoid cycles, allowing label propagation in topological order.
- Many practical problems can be modeled as DAGs: project scheduling (precedence constraints), layered networks, time-expanded networks.
- Exact dynamic programming is often feasible with budgets that are not excessively large.

Features
--------
- Clear implementation of a resource-constrained shortest path solver for DAGs.
- Support for:
  - exact dynamic-programming / label-setting approach,
  - pruning dominated labels,
  - optional path reconstruction.
- Configurable cost/resource types (int/float).
- CLI for running experiments and examples.
- Utilities for generating synthetic DAGs and visualizing results.

Algorithm & Complexity
----------------------
High-level algorithm (label propagation):
1. Compute a topological order of vertices.
2. For the source s, create an initial label: (cost=0, resource=0, pred=None).
3. Process vertices in topological order:
   - For each label at u, relax along outgoing edges (u→v), producing new labels at v with updated (cost, resource).
   - Discard labels whose resource > B.
   - Apply dominance pruning: label A dominates label B if A.cost ≤ B.cost and A.resource ≤ B.resource (with one strict).
4. At the target t, pick the label with minimum cost among those with resource ≤ B and reconstruct path using predecessors.

Time and memory complexity:
- Let |V| = n, |E| = m, and budget discretization size = R (e.g., integer B or quantized).
- Worst-case: O(m * L) where L is average number of non-dominated labels per node.
- In practice, for DAGs and modest budgets, L remains small due to pruning.
- If resource is integer and budget B, DP table can be O(n * B).

Input / Output Formats
----------------------
The repository includes flexible input readers. The canonical JSON graph format:

{
  "nodes": ["A", "B", "C", ...],
  "edges": [
    {"u": "A", "v": "B", "cost": 3.2, "resource": 1.0},
    {"u": "B", "v": "C", "cost": 2.5, "resource": 2.0}
  ],
  "source": "A",
  "target": "C",
  "budget": 5.0
}

Outputs:
- Best path as a list of nodes, total cost, total resource.
- Optionally a JSON with all non-dominated labels at each node for analysis.

Quickstart
----------
Prerequisites
- Python 3.8+
- Recommended: create a virtual environment

Install (example)
```bash
python -m venv venv
source venv/bin/activate  # on Windows: venv\Scripts\activate
pip install -r requirements.txt
```

Run a solver (if CLI script is `solve.py`)
```bash
python solve.py --input examples/sample_graph.json --budget 10.0 --output results.json
```

If your repo uses a different entrypoint, adapt the command accordingly.

Examples
--------
Example 1 — simple graph:
- Input: examples/simple.json
- Command:
  python solve.py --input examples/simple.json --budget 5
- Expected output:
  Path: A -> B -> D -> target
  Cost: 12.3
  Resource: 4

Example 2 — generated DAG with random weights:
- Generate and solve:
  python generate_graph.py --nodes 100 --layers 10 --seed 42 --out graphs/rand100.json
  python solve.py --input graphs/rand100.json --budget 50.0

Benchmarks & Experiments
------------------------
- scripts/benchmark.py runs experiments varying node counts, edge densities, and budgets.
- Store results in CSV for plotting.
- Suggested plots:
  - runtime vs. budget
  - average labels per node vs. budget
  - cost vs. budget tradeoff curve

Testing
-------
- Unit tests live in tests/
- Run tests:
  pytest -q

Development & Contribution
--------------------------
Contributions are welcome! A suggested workflow:
1. Fork the repository.
2. Create a feature branch: git checkout -b feat/your-feature
3. Add tests for new functionality.
4. Open a pull request describing the change.

Please follow the coding style:
- Python: black + flake8 recommended.
- Include docstrings and small, focused commits.

File / Module Map (example)
- solve.py — CLI entrypoint (parses args, loads graph, calls solver)
- solver.py — core algorithm (label propagation)
- graph_io.py — readers/writers for JSON / CSV graphs
- utils.py — helpers (topological order, dominance checks)
- examples/ — sample graphs
- tests/ — unit & integration tests
- scripts/ — generate_graph.py, benchmark.py

Notes & Tips
------------
- If resource values are floating-point, consider quantization to integers for efficient DP pruning.
- If budget is very large, consider heuristics or limits on labels (e.g., beam pruning).
- If the DAG is layered (time-expanded), exploit layers to further constrain label propagation.

Acknowledgements
----------------
Thanks to contributors and the open-source community for algorithmic resources and examples.

Contact
-------
Repo: https://github.com/64Sanjay/Shortest-path-with-resource-constraints-in-DAG
Author / Maintainer: 64Sanjay
