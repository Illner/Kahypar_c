#include <iostream>
#include <memory>
#include <vector>

#include "libkahypar.h"

int main(int argc, char *argv[]) {

  kahypar_context_t *context = kahypar_context_new();
  kahypar_configure_context_from_file(context, "config/cut_kKaHyPar_sea20.ini");

  kahypar_set_seed(context, 42);

  const kahypar_hypernode_id_t num_vertices = 7;
  const kahypar_hyperedge_id_t num_hyperedges = 4;

  std::unique_ptr<kahypar_hyperedge_weight_t[]> hyperedge_weights =
      std::make_unique<kahypar_hyperedge_weight_t[]>(4);

  // force the cut to contain hyperedge 0 and 2
  hyperedge_weights[0] = 1;
  hyperedge_weights[1] = 1000;
  hyperedge_weights[2] = 1;
  hyperedge_weights[3] = 1000;

  std::unique_ptr<size_t[]> hyperedge_indices = std::make_unique<size_t[]>(5);

  hyperedge_indices[0] = 0;
  hyperedge_indices[1] = 2;
  hyperedge_indices[2] = 6;
  hyperedge_indices[3] = 9;
  hyperedge_indices[4] = 12;

  std::unique_ptr<kahypar_hyperedge_id_t[]> hyperedges =
      std::make_unique<kahypar_hyperedge_id_t[]>(12);

  // hypergraph from hMetis manual page 14
  hyperedges[0] = 0;
  hyperedges[1] = 2;
  hyperedges[2] = 0;
  hyperedges[3] = 1;
  hyperedges[4] = 3;
  hyperedges[5] = 4;
  hyperedges[6] = 3;
  hyperedges[7] = 4;
  hyperedges[8] = 6;
  hyperedges[9] = 2;
  hyperedges[10] = 5;
  hyperedges[11] = 6;

  const double imbalance = 0.03;
  const kahypar_partition_id_t k = 2;

  kahypar_hyperedge_weight_t objective = 0;

  std::vector<kahypar_partition_id_t> partition(num_vertices, -1);

  kahypar_partition(num_vertices, num_hyperedges, imbalance, k,
                    /*vertex_weights */ nullptr, hyperedge_weights.get(),
                    hyperedge_indices.get(), hyperedges.get(), &objective,
                    context, partition.data());

  for (int i = 0; i != num_vertices; ++i) {
    std::cout << i << ":" << partition[i] << std::endl;
  }

  kahypar_context_free(context);
}
