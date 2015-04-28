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
#include "TranslatorImp.h"

#include <sstream>
#include <fstream>
#include <utility>
#include <assert.h>
#include <stdlib.h>
#include <libgen.h>
#include <antlr3.h>

#include "json.h"
#include "ID.h"
#include "AttributeManager.h"
#include "QueryManager.h"
#include "Catalog.h"
#include "QueryExit.h"
#include "DataPathGraph.h"
#include "TransMessages.h"
#include "ExprListInfo.h"
#include "ExternalCommands.h"

/** this has to be last */
#include "PiggyLexer.h"
#include "PiggyParser.h"
#include "DPtree.h"

/* This is needed by the tree grammar to count variables */
int ExprListInfo::cVarCNT = 0;

using namespace std;


TranslatorImp::TranslatorImp(EventProcessor& _coordinator, bool batchMode) :
    coordinator()
    , lTrans(batchMode)
#ifdef  DEBUG_EVPROC
  ,EventProcessorImp(true, "Translator")
#endif
{
    coordinator.copy(_coordinator);

    // register all the relations as waypoints so they can be used as scanners
    // Catalog and definition of file scanners
    Catalog& catalog=Catalog::GetCatalog();

    StringContainer relations = catalog.GetRelationNames();
    for (int i=0;  i<relations.size(); i++){
        string relName=relations[i];

        // we do not have real scanners(i.e. we do not start FileScanner)
        // we just define the WayPointIDs for all the scanners
        WayPointID id(relName);
    }


    

}

/** this is a temporary function to generate a graph */


/** when we process this message, we assume that the coordinator already
        read the catalog and initialized the attribute manager from it
*/

TranslatorImp::Translate(TranslationMessage &msg) {

    // get the directory name based on the file passed to the translator
    char param[1000];
    strcpy (param, msg.confFile.c_str());
    string outDir = string(dirname(param));
    outDir = "./Generated/";

    /** If the parser works, the next line should be used
            Otherwise, one of the hardwired plans should be used
     */

    cout << "PARSING FILE " << msg.confFile << endl;

    bool success=evProc.ParseFile(msg.confFile.c_str());
    if (!success){
        cout << "PARSING FAILED" << endl;
        // TODO: send a message that parsing failed
        exit(EXIT_FAILURE);
        return;
    }

    // run one of the programs to have a graph
    //     evProc.LoadLineitem();
    //   evProc.EightQueries();
    //     evProc.Q1();
    // SimpleJoinAgg();

    DataPathGraph myGraph;
    WayPointConfigurationList waypoints;
    QueryExitContainer newQueries;
    TaskList tasks;

    if (evProc.lTrans.GetConfig(outDir, newQueries, myGraph, waypoints, tasks)){

        ofstream file;
        file.open("DOT.dot");
        evProc.lTrans.PrintDOT(file);
        // generate a link to the last config


#if 0
        sprintf (param, "ln -s %s lgen", outDir.c_str());
        execute_command ("mv prev old");
        execute_command ("mv lgen prev");
        execute_command ("rm -f lgen");
        execute_command (param);
#endif

        SymbolicQueryDescriptions_Factory(evProc.coordinator,    newQueries, myGraph, waypoints, tasks);
    } else {
        // no plan yet
    }

}


TranslatorImp::DeleteQueries(DeleteQueriesMessage &msg) {

    // we just need to tell the LemonTranslator to drop queries
    QueryIDSet queries = msg.queries;
    while (!queries.IsEmpty()){
        evProc.lTrans.DeleteQuery(queries.GetFirst());
    }

}


 /* The next two functions have a lot of overlap. Since the Antlr code
    is C not C++, it is very hard to factorize the code corectly. An
    attempt to cut the code before the tree-grammar failed
    miserably.
 */

bool  TranslatorImp::ParsePiggy(    pANTLR3_INPUT_STREAM    input){
    pPiggyLexer            lxr;
    pANTLR3_COMMON_TOKEN_STREAM        tstream;
    pPiggyParser                psr;
    PiggyParser_parse_return langAST;
    pANTLR3_COMMON_TREE_NODE_STREAM nodes;
    pDPtree       treePsr;

    // initialize lexer
    lxr    = PiggyLexerNew(input);        // TLexerNew is generated by ANTLR
    FATALIF( lxr == NULL, "Unable to create the lexer due to malloc() failure1\n");

    // initialize input stream
    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));
    FATALIF(tstream == NULL, "Out of memory trying to allocate token stream\n");

    // start parser
    psr    = PiggyParserNew(tstream);  // PiggyParserNew is generated by ANTLR3
    FATALIF(psr == NULL, "Out of memory trying to allocate parser\n");

    langAST = psr->parse(psr, &lTrans);
    if (psr->pParser->rec->state->errorCount > 0)
    {
        fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n", psr->pParser->rec->state->errorCount);

        printf("Nodes so far: %s\n", langAST.tree->toStringTree(langAST.tree)->chars);

        return false;
    }

    nodes = antlr3CommonTreeNodeStreamNewTree(langAST.tree, ANTLR3_SIZE_HINT);
    //printf("Nodes: %s\n", langAST.tree->toStringTree(langAST.tree)->chars);

    treePsr = DPtreeNew(nodes);
    treePsr->parse(treePsr, &lTrans, (const char *) input->fileName->chars);
    treePsr ->free  (treePsr);      treePsr = NULL;

    psr         ->free  (psr);          psr = NULL;
    tstream     ->free  (tstream);      tstream = NULL;
    lxr         ->free  (lxr);          lxr = NULL;

    return true;
}

/* return false if fail */
bool TranslatorImp::ParseFile(const char* filename){
    // figure out the type of file from the extension
    const char* dotLoc = strrchr(filename, '.');

    // First preprocess file using C++ processor.
    string inFile = filename;

    bool result = true;
    pANTLR3_UINT8        fName;
    pANTLR3_INPUT_STREAM    input;

    input    = antlr3FileStreamNew((pANTLR3_UINT8)inFile.c_str(), ANTLR3_ENC_8BIT);

    if (input == NULL){
        WARNING( "Failed to open the file %s\n", inFile.c_str());
        return false;
    }

    if (strcmp(dotLoc, ".pgy") == 0) { // piggy fil
        cout << "Using Piggy parser" << endl;
        result = TranslatorImp::ParsePiggy(input);
    } else {
        cout << "Got a file I do not know what to do with: " << filename << endl;
        return false; // what are we parsing?
    }

    input   ->close (input);        input = NULL;

    return result;
}
