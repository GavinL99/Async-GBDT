
#ifndef _GBDT_H_
#define _GBDT_H_

#include "tree_seq.hpp"
#include "concurrency.h"
#include <thread>

namespace gbdt {
  class GBDT {
  public:
    GBDT(Configure conf) : trees(NULL),
                           bias(0),
                           conf(conf),
                           gain(NULL) {
      shrinkage = conf.shrinkage;
    }

    void Fit_OMP(DataVector *d);

    void Fit_Async(DataVector *d);

    std::string Save(bool);

    void Load(const std::string &s);

    double *GetGain() { return gain; }

    ValueType Predict(const Tuple &t) const;

    ~GBDT();

  private:
    void Init(DataVector &d);

    double GetLoss(DataVector *d, size_t samples, int i, std::vector<ValueType> temp_pred);

    double GetLossSimple(DataVector *d, size_t samples, std::vector<ValueType> temp_pred);

    ValueType Predict_OMP(const Tuple &t, size_t n, ValueType temp_pred) const;

    ValueType PredictAsync(const Tuple &t, RegressionTree *tree, ValueType temp_pred) const;

    void WorkerSide(int dsize);

    void ServerSide(int dsize, std::vector <ValueType> &temp_pred);


    void ReleaseTrees() {
      if (trees) {
        for (size_t i = 0; i < conf.num_trees; ++i) {
          delete trees[i];
        }
        delete[] trees;
        trees = NULL;
      }
    }

  private:
    RegressionTree **trees;
    // for trees
    ValueType bias;
    ValueType shrinkage;

    // for async concurrency
    DataVector *data_ptr_;
    ReaderWriterLatch data_ptr_lock_;
    ConcurrentVector<RegressionTree> trees_vec_;
    bool server_finish_{false};

    Configure conf;

    double *gain;

    DISALLOW_COPY_AND_ASSIGN(GBDT);
  };
}

#endif /* _GBDT_H_ */
