#include "AvailableExprAnalysis.h"
#include "llvm/Support/raw_ostream.h"

AvailableExprAnalysis::AvailableExprAnalysis(Instruction * inst, AvailableExprLattice * incoming) {
	_instruction = inst;
	ExpressionContainer empty;
	_incomingEdge = new AvailableExprLattice(false,true,empty);
	*_incomingEdge = *incoming;
	_outgoingEdge = new AvailableExprLattice(false,true,empty);
}

void AvailableExprAnalysis::applyFlowFunction() {
    if (_instruction->isBinaryOp()) {
        handleBinaryOp(_instruction);            
    }
	else {//temp 
		*_outgoingEdge = *_incomingEdge;
	}
}

Instruction * AvailableExprAnalysis::getInstruction() {
    return _instruction;
}

AvailableExprLattice * AvailableExprAnalysis::getOutgoingEdge() {
    return _outgoingEdge;
}

void AvailableExprAnalysis::setIncomingEdge(AvailableExprLattice * incoming) {    
    *_incomingEdge = *incoming;
}

// will be a join
AvailableExprLattice * AvailableExprAnalysis::merge(AvailableExprLattice * edge_1, AvailableExprLattice * edge_2) {
	ExpressionContainer outgoingEdge;
	if (edge_1->isTop() || edge_2->isTop()) {
		return new AvailableExprLattice(true, false, outgoingEdge); // return Top
	}
	else if (edge_1->isBottom() && edge_2->isBottom()) {
		return new AvailableExprLattice(false, true, outgoingEdge);
	}
	else if (edge_1->isBottom()) {
		return new AvailableExprLattice(false, false, edge_2->getFacts());
	}
	else if (edge_2->isBottom()) {
		return new AvailableExprLattice(false, false, edge_1->getFacts());
	}
	map<string,Expression*> edge1 = edge_1->getFacts().getMap();
	map<string,Expression*> edge2 = edge_2->getFacts().getMap();

    for (map<string,Expression*>::iterator i = edge1.begin(); i != edge1.end(); i++) {
        //bool isEqualInBothEdges = false;
        for (map<string,Expression*>::iterator j = edge2.begin(); j != edge2.end(); j++) {
            // If item is in both edges and are equal, add to outgoing edge
            /*
            if (i->first->isEqual(j->first)) {
                vector<string> v(i->second.size() + j->second.size());
                vector<string>::iterator it;

                it = set_union(i->second.begin(), i->second.end(), j->second.begin(), j->second.end(), v.begin());
                v.resize(it-v.begin());
                outgoingEdge[i->first] = v;
                break;
            }
            */
        }
    }
    
    return new AvailableExprLattice(false, false, outgoingEdge);
}

bool AvailableExprAnalysis::equal(AvailableExprLattice * edge_1, AvailableExprLattice * edge_2) {
	if(edge_1->isTop() && edge_2->isTop()) 
		return true;
	else if(edge_1->isBottom() && edge_2->isBottom())
		return true;
	map<string,Expression*> edge1 = edge_1->getFacts().getMap();
	map<string,Expression*> edge2 = edge_2->getFacts().getMap();
    if(edge1.size() != edge2.size())
        return false;
    for (map<string,Expression*>::iterator i = edge1.begin(); i != edge1.end(); i++) {
        if (edge1[i->first] != edge2[i->first])
            return false;
    }

    return true;
}

void AvailableExprAnalysis::dump() {
    errs() << "\t\t\tINCOMING:\n";
    _incomingEdge->dump();

    errs() << "\t\t\tOUTGOING:\n";
    _outgoingEdge->dump();
    
    errs() << "\t\t--------------------------------------------------------\n";
}

void AvailableExprAnalysis::handleBinaryOp(Instruction * inst) {
    ExpressionContainer edgeMap = _incomingEdge->getFacts();

    edgeMap.addExpression(inst->getOperandUse(0).getUser()->getName().str(), new Expression(inst));
    //edgeMap[new Expression(inst)].push_back(inst->getOperandUse(0).getUser()->getName().str());

    _outgoingEdge->setNewFacts(false,false,edgeMap);
}

