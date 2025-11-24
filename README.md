# Document Selection Algorithms

C++ implementation of various document selection algorithms for information retrieval evaluation using TREC datasets.

## Algorithms

- **MTF (Move-to-Front)**: Baseline document selection with variants (MTF1, MTF2)
- **Hedge**: Exponential weighting algorithm for online learning
- **Algo1-16**: Custom document selection strategies with different weighting schemes and NDCG optimization

## Building

```bash
make
```

## Usage

```bash
./algo -q <qrel_path> -r <runs_path> -o <output_path>
```

## Datasets

- TREC Web Track 2013/2014
- TREC Robust 2003
- TREC-7

## Output

Results are generated for varying judge counts (50-500) and include NDCG scores for each algorithm configuration.
