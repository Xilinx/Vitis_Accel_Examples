/**
* Copyright (C) 2020 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/
#ifndef _XF_FINTECH_MCENGINE_H_
#define _XF_FINTECH_MCENGINE_H_
/**
 * @file mc_engine.hpp
 * @brief This file includes implementation of pricing engine for different
 * option.
 */

#include "xf_fintech/bs_model.hpp"
#include "xf_fintech/mc_simulation.hpp"
#include "xf_fintech/rng.hpp"
namespace xf {

namespace fintech {

using namespace internal;
#define MAX_SAMPLE 134217727
/**
 * @brief European Option Pricing Engine using Monte Carlo Method. This
 * implementation uses Black-Scholes valuation model.
 *
 * @tparam DT supported data type including double and float data type, which
 * decides the precision of result, default double-precision data type.
 * @tparam UN number of Monte Carlo Module in parallel, which affects the
 * latency and resources utilization, default 10.
 * @tparam Antithetic antithetic is used  for variance reduction, default this
 * feature is disabled.
 * @param underlying intial value of underlying asset at time 0.
 * @param volatility fixed volatility of underlying asset.
 * @param dividendYield the constant dividend rate for continuous dividends.
 * @param riskFreeRate risk-free interest rate.
 * @param timeLength the time length of contract from start to end.
 * @param strike the strike price also known as exericse price, which is settled
 * in the contract.
 * @param optionType option type. 1: put option, 0: call option.
 * @param seed array to store the inital seed for each RNG.
 * @param output output array.
 * @param requiredTolerance the tolerance required. If requiredSamples is not
 * set, when reaching the required tolerance, simulation will stop, default
 * 0.02.
 * @param requiredSamples the samples number required. When reaching the
 * required number, simulation will stop, default 1024.
 * @param timeSteps the number of discrete steps from 0 to T, T is the expiry
 * time, default 100.
 * @param maxSamples the maximum sample number. When reaching it, the simulation
 * will stop, default 2,147,483,648.
 */
template <typename DT = double, int UN = 10, bool Antithetic = false>
void MCEuropeanEngine(DT underlying,
                      DT volatility,
                      DT dividendYield,
                      DT riskFreeRate, // model parameter
                      DT timeLength,
                      DT strike,
                      bool optionType, // option parameter
                      ap_uint<32>* seed,
                      DT* output,
                      DT requiredTolerance = 0.02,
                      unsigned int requiredSamples = 1024,
                      unsigned int timeSteps = 100,
                      unsigned int maxSamples = MAX_SAMPLE) {
    // number of samples per simulation
    const static int SN = 1024;

    // number of variate
    const static int VN = 1;

    // Step first or sample first for each simulation
    const static bool SF = true;

    // option style
    const OptionStyle sty = European;

    // antithetic enable or not
    // const static bool Antithetic = false;

    // RNG alias name
    typedef MT19937IcnRng<DT> RNG;

    BSModel<DT> BSInst;

    // path generator instance
    BSPathGenerator<DT, SF, SN, Antithetic> pathGenInst[UN][1];
#pragma HLS array_partition variable = pathGenInst dim = 1

    // path pricer instance
    PathPricer<sty, DT, SF, SN, Antithetic> pathPriInst[UN][1];
#pragma HLS array_partition variable = pathPriInst dim = 1

    // RNG sequence instance
    RNGSequence<DT, RNG> rngSeqInst[UN][1];
#pragma HLS array_partition variable = rngSeqInst dim = 1

    // pre-process for "cold" logic.
    DT dt = timeLength / timeSteps;
    DT f_1 = internal::FPTwoMul(riskFreeRate, timeLength);
    DT discount = internal::FPExp(-f_1);

    BSInst.riskFreeRate = riskFreeRate;
    BSInst.dividendYield = dividendYield;
    BSInst.volatility = volatility;
    //
    BSInst.variance(dt);
    BSInst.stdDeviation();
    BSInst.updateDrift(dt);

    // configure the path generator and path pricer
    for (int i = 0; i < UN; ++i) {
#pragma HLS unroll
        // Path generator
        pathPriInst[i][0].optionType = optionType;
        pathPriInst[i][0].strike = strike;
        pathPriInst[i][0].underlying = underlying;
        pathPriInst[i][0].discount = discount;
        // Path pricer
        pathGenInst[i][0].BSInst = BSInst;
        // RNGSequnce
        rngSeqInst[i][0].seed[0] = seed[i];
    }

    // call monter carlo simulation
    DT price = mcSimulation<DT, RNG, BSPathGenerator<DT, SF, SN, Antithetic>, PathPricer<sty, DT, SF, SN, Antithetic>,
                            RNGSequence<DT, RNG>, UN, VN, SN>(timeSteps, maxSamples, requiredSamples, requiredTolerance,
                                                              pathGenInst, pathPriInst, rngSeqInst);

    // output the price of option
    output[0] = price;
}

#undef MAX_SAMPLE
} // namespace fintech
} // namespace xf
#endif // ifndef _XF_FINTECH_MCENGINE_H_
