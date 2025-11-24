# Document Selection Algorithms

C++ implementation of various document selection algorithms for information retrieval evaluation using TREC datasets. This project evaluates different strategies for selecting which documents to judge in order to maximize system ranking correlation with minimal human judgment effort.

## Overview

This repository contains implementations of document selection algorithms that optimize the trade-off between judgment cost and evaluation accuracy. The algorithms select documents strategically to judge, aiming to accurately rank retrieval systems while minimizing the number of relevance judgments required.

## Algorithms

- **MTF (Move-to-Front)**: Baseline document selection strategy with variants
  - MTF1: Basic move-to-front approach
  - MTF2: Enhanced variant with improved selection criteria
- **Hedge**: Exponential weighting algorithm for online learning-based document selection
- **Algo1-16**: Suite of custom document selection strategies featuring:
  - Different weighting schemes for system selection
  - NDCG-based optimization
  - Adaptive pooling strategies

## Prerequisites

- C++ compiler with C++11 support
- Python 3.x (for result analysis)
- Python packages: `scipy`, `pandas`, `numpy`

## Building

Compile the C++ algorithms:

```bash
make
```

This will generate the `algo` executable.

## Usage

### Running Algorithms

```bash
./algo -q <qrel_path> -r <runs_path> -o <output_path>
```

**Parameters:**
- `-q <qrel_path>`: Path to the TREC qrels (relevance judgments) file
- `-r <runs_path>`: Path to directory containing system run files
- `-o <output_path>`: Output directory for generated judgments

### Analyzing Results

Run the Python analysis script to evaluate algorithm performance:

```bash
python get_results.py
```

This script:
- Evaluates system runs using `trec_eval`
- Computes Kendall's tau correlation between predicted and official rankings
- Generates performance metrics for varying judgment counts (50-500)

## Datasets

The project supports multiple TREC datasets:

- **TREC Web Track 2013/2014**: Web search evaluation
- **TREC Robust 2003/2004**: Robust retrieval track
- **TREC-7/8**: Ad-hoc retrieval tasks

Each dataset should include:
- Qrels files (relevance judgments)
- System run files (in `inputfiles/` or `runs/` subdirectory)

## Output

The algorithms generate:

1. **Judgment files** (`new_judgments/`): Selected documents for manual judging
2. **Qrels files** (`new_qrels/`): Simulated relevance judgments
3. **Performance metrics** (`test_scores/`): Kendall's tau correlation scores
4. **System results** (`system_run_results/`): MAP scores for each system

Results are generated for varying judge counts (50, 100, 150, ..., 500) to analyze the trade-off between judgment effort and ranking accuracy.

## Evaluation Metrics

- **MAP (Mean Average Precision)**: Primary evaluation metric for system effectiveness
- **Kendall's tau**: Measures rank correlation between predicted and official system rankings
- **NDCG**: Used internally by some algorithms for document selection

