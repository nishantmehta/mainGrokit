//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#ifndef _COORDINATOR_IMP_H_
#define _COORDINATOR_IMP_H_

#include "ID.h"
#include "EventProcessorImp.h"
#include "DataPathGraph.h"
#include "ContainerTypes.h"
#include "Timer.h"
#include "Constants.h"
#include "TransMessages.h"
#include "Tasks.h"
#include "DPMessages.h"
#include "CLMessages.h"


/** This coordinator is designed to go together with the Translato

  It is fairly general but needs to be generalized a little more to
  be good for the final translator.

BEHAVIOR: this coordinator is a one-shoot coordinator. It uses a
single set of configuration files and it runs the queries in them
to completion then stops. The complete coordinator needs an
interface to get the query descriptions or, eventually, the SQL
statements.

*/

class CoordinatorImp : public EventProcessorImp {
    private:
        // keep track of the execution engine
        EventProcessor execEngine;
        // the translator (simple translator here)
        EventProcessor translator;
        // the code generator
        EventProcessor codeLoader;
        // the file scanners (we have to keep them otherwise they get deleted
        EventProcessorContainer scanners;

        // the current graph (saved until the code is compiled)
        DataPathGraph cachedGraph;

        TaskList newTasks;

        char lastDir[1000];

        // timing facility
        Timer clock;

        // start times for all the queries
        // used to determine running times for queries
        typedef Swapify<double> SW_double;
        EfficientMap<QueryID, SW_double > startTimes;

        bool compileOnly;

        // Whether or not to quit immediately after query completion
        static bool quitWhenDone;

        // Private methods
        void Quit();

    public:
        // constructor
        // metadataFile is the file containing metadata about the relation being read
        // graphFile is the file containing the metadata describing the graph and waypoints
        // waypointFile is a file containing table scan and waypoint detailed info
        CoordinatorImp( bool _quitWhenDone, bool batchMode, bool compileOnly );

        // destructor doing nothing
        virtual ~CoordinatorImp(void){ }

        void DieProc(DieMessage &msg);
        void QueryFinishedProc(QueriesDoneMessage &msg);
        void SymbolicQueriesProc(SymbolicQueryDescriptions &msg);
        void CodeProc(LoadedCodeMessage &msg);
        void NewPlanProc(NewPlan &msg);

        ACTOR_HANDLE
            // kill message; need to take everything down
            HANDLER(DieMessage, DieProc, 1)
            // the query ended
            // processes message QueryFinished
            HANDLER(QueriesDoneMessage, QueryFinishedProc, 1)
            // message to process the symbolic info from the Scheduler
            HANDLER(SymbolicQueryDescriptions, SymbolicQueriesProc, 2)
            // message handler for the compiled code from the CodeGenerator
            HANDLER(LoadedCodeMessage, CodeProc, 3)
            // new plan from the external planner
            HANDLER(NewPlan, NewPlanProc, 4)
        END_HANDLE
};


#endif // _COORDINATOR_H_
