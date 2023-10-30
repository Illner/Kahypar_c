#include <iostream>
#include <memory>
#include <vector>

#include "libkahypar.h"

#include "config/cut_kKaHyPar_sea20.hpp"

#include <stdio.h>
#include <unistd.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>


int memReadStat(int field)
{
  char  name[256];
  pid_t pid = getpid();
  int   value;

  sprintf(name, "/proc/%d/statm", pid);
  FILE* in = fopen(name, "rb");
  if (in == NULL) return 0;

  for (; field >= 0; field--)
    if (fscanf(in, "%d", &value) != 1)
      printf("ERROR! Failed to parse memory statistics from \"/proc\".\n"), exit(1);
  fclose(in);

  std::cout << "/proc/%d/statm (value): " << std::to_string(value) << std::endl;

  return value;
}

double memUsed() { return (double)memReadStat(0) * (double)getpagesize() / (1024*1024); }

using MemorySizeType = long double;   // 128 bits

const std::string VM_SIZE_START_OF_LINE = "VmSize:";
const std::string VM_PEAK_START_OF_LINE = "VmPeak:";

constexpr MemorySizeType NOT_SUPPORTED_VIRTUAL_MEMORY_SIZE_VALUE = static_cast<MemorySizeType>(-1);

MemorySizeType readProcSelfStatusFile(const std::string& startLine) {
  assert(startLine == VM_SIZE_START_OF_LINE || startLine == VM_PEAK_START_OF_LINE);   // valid start of the line

  const std::string fileName = "/proc/self/status";
  MemorySizeType memorySize = NOT_SUPPORTED_VIRTUAL_MEMORY_SIZE_VALUE;

  // The file does not exist
  if (!std::filesystem::exists(fileName))
    return memorySize;

  {
    std::ifstream fileStream(fileName, std::ios::in);

    // The file cannot be opened
    if (!fileStream.is_open())
      return memorySize;

    std::string line;
    while (std::getline(fileStream, line)) {
      if (line.starts_with(startLine)) {
        std::cout << line << std::endl;
        break;
      }
    }
  }

  return memorySize;
}

MemorySizeType getCurrentVirtualMemorySize() {
  return readProcSelfStatusFile(VM_SIZE_START_OF_LINE);
}


int main(int argc, char *argv[]) {

  kahypar_context_t *context = kahypar_context_new();
  // kahypar_configure_context_from_file(context, "config/cut_kKaHyPar_sea20.ini");
  kahypar_configure_context_from_string(context, cut_kKaHyPar_sea20_config);

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

  std::cout << "Minisat: " << std::to_string(memUsed()) << " MB" << std::endl;
  std::cout << "Bella: ";
  getCurrentVirtualMemorySize();
}
