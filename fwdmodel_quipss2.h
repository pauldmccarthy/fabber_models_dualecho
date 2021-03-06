/*  fwdmodel_quipss2.h - Implements the QUIPSS II ASL model

    Adrian Groves & Michael Chappell, QuBIc (IBME) & FMRIB Image Analysis Group

    Copyright (C) 2007-2015 University of Oxford  */

/*  CCOPYRIGHT */

/* fwdmodel_quipss2.h
 * Class declaration for the QUIPSS II ASL model used in Woolrich MRM'06
 * with additional terms for nuisance regressors (affecting the signal). 
 * Written by Adrian Groves, 2007
 * FMRIB Centre, University of Oxford
 *
 */

#ifndef __FABBER_FWDMODEL_QUIPSS2_H
#define __FABBER_FWDMODEL_QUIPSS2_H 1

#include "fabber_core/fwdmodel.h"
#include <string>
using namespace std;

class Quipss2FwdModel : public FwdModel
{
public:
    static FwdModel *NewInstance();

    // Virtual function overrides
    virtual void Initialize(ArgsType &args);
    virtual void Evaluate(const NEWMAT::ColumnVector &params,
        NEWMAT::ColumnVector &result) const;
    virtual vector<string> GetUsage() const;
    virtual void DumpParameters(const NEWMAT::ColumnVector &vec,
        const string &indents = "") const;
    virtual void GetOptions(std::vector<OptionSpec> &opts) const;
    virtual std::string GetDescription() const;

    virtual void NameParams(vector<string> &names) const;
    virtual int NumParams() const
    {
        return NnStart(echoTime.Ncols() + 1) + (stdevT1b > 0 ? 1 : 0)
            + (stdevInvEff > 0 ? 1 : 0) + (stdevDt > 0 ? 1 : 0) - 1;
    }

    virtual string ModelVersion() const;

    virtual ~Quipss2FwdModel() { return; }
    virtual void HardcodedInitialDists(MVNDist &prior, MVNDist &posterior) const;

protected: // Constants
    int Q0index() const { return 1; }
    int M0index() const { return Q0index() + Qbasis.Ncols() + 1; }
    int R0index() const { return M0index() + Mbasis.Ncols() + 1; }
    int NnStart(int te) const
    {
        assert(te > 0 && te <= echoTime.Nrows() + 1); // go one past, for getting end point
        return R0index() + Rbasis.Ncols() + (te - 1) * Nbasis.Ncols() + 1;
    }
    int InvEffIndex() const
    {
        assert(stdevInvEff > 0);
        return NnStart(echoTime.Ncols() + 1);
    }
    int T1bIndex() const
    {
        assert(stdevT1b > 0);
        return NnStart(echoTime.Ncols() + ((stdevInvEff > 0) ? 1 : 0));
    }
    int dtIndex() const
    {
        assert(stdevDt > 0);
        return NnStart(echoTime.Ncols() + 1) + (stdevT1b > 0 ? 1 : 0) + (stdevInvEff > 0 ? 1 : 0);
    }

    // Slow submatrixing... but it works.
    // In fact, it's plenty fast enough.. fwd model is a small part calculation time
    NEWMAT::ReturnMatrix QnOf(const NEWMAT::ColumnVector &params) const
    {
        return params.Rows(Q0index() + 1, Q0index() + Qbasis.Ncols()).Evaluate();
    }
    NEWMAT::ReturnMatrix MnOf(const NEWMAT::ColumnVector &params) const
    {
        return params.Rows(M0index() + 1, M0index() + Mbasis.Ncols()).Evaluate();
    }
    NEWMAT::ReturnMatrix RnOf(const NEWMAT::ColumnVector &params) const
    {
        return params.Rows(R0index() + 1, R0index() + Rbasis.Ncols()).Evaluate();
    }
    NEWMAT::ReturnMatrix NnOf(int te, const NEWMAT::ColumnVector &params) const
    {
        return params.Rows(NnStart(te), NnStart(te + 1) - 1).Evaluate();
    }

    // scan parameters
    NEWMAT::ColumnVector rho;
    NEWMAT::ColumnVector echoTime;
    double TI1, TI2;

    // assumed known constants
    double fixedInvEff, fixedT1b, fixedDt;
    double stdevT1b, stdevInvEff, stdevDt; // >0 means treat these as parameters!

    // basis for each piece
    NEWMAT::Matrix Qbasis; // basis in columns?
    NEWMAT::Matrix Mbasis;
    NEWMAT::Matrix Rbasis;
    NEWMAT::Matrix Nbasis;

private:
    /** Auto-register with forward model factory. */
    static FactoryRegistration<FwdModelFactory, Quipss2FwdModel> registration;
};

#endif /* __FABBER_FWDMODEL_QUIPSS2_H */
