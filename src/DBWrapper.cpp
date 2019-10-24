#include "DBWrapper.h"

#include <iostream>
#include <vector>

#include "db.h"
#include "lefin.h"
#include "defin.h"
#include "defout.h"
#include "dbShape.h"
#include "Coordinate.h"

DBWrapper::DBWrapper(Netlist& netlist, Core& core, Parameters& parms) : 
                _netlist(&netlist), _core(&core), _parms(&parms) {
        _db = ads::dbDatabase::create();
}

void DBWrapper::parseLEF(const std::string &filename) {
        ads::lefin lefReader(_db, false);
        lefReader.createTechAndLib("testlib", filename.c_str());
}

void DBWrapper::parseDEF(const std::string &filename) {
        ads::defin defReader(_db);

        std::vector<ads::dbLib *> searchLibs;
        ads::dbSet<ads::dbLib> libs = _db->getLibs();
        ads::dbSet<ads::dbLib>::iterator itr;
        for(itr = libs.begin(); itr != libs.end(); ++itr) {
                searchLibs.push_back(*itr);
        }
        
        _chip = defReader.createChip(searchLibs, filename.c_str());
}

void DBWrapper::populateIOPlacer() {
        initNetlist();
        initCore();
}


void DBWrapper::initCore() {
        ads::dbTech* tech = _db->getTech();
        if (!tech) {
                std::cout << "[ERROR] ads::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }

        int databaseUnit = tech->getLefUnits(); 

        ads::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] ads::dbBlock not found! Exiting...\n";
                std::exit(1);
        }

        ads::dbBox* coreBBox = block->getBBox();

        Coordinate lowerBound(coreBBox->xMin(), coreBBox->yMin());
        Coordinate upperBound(coreBBox->xMax(), coreBBox->yMax());
        
        int horLayerIdx = _parms->getHorizontalMetalLayer();
        int verLayerIdx = _parms->getVerticalMetalLayer();

        ads::dbTechLayer* horLayer = tech->findRoutingLayer(horLayerIdx);
        if (!horLayer) {
                std::cout << "[ERROR] Layer" << horLayerIdx << " not found! Exiting...\n";
                std::exit(1);
        }

        ads::dbTechLayer* verLayer = tech->findRoutingLayer(verLayerIdx);
        if (!horLayer) {
                std::cout << "[ERROR] Layer" << verLayerIdx << " not found! Exiting...\n";
                std::exit(1);
        }

        ads::dbTrackGrid* horTrackGrid = block->findTrackGrid( horLayer );        
        ads::dbTrackGrid* verTrackGrid = block->findTrackGrid( verLayer );
        if (!horTrackGrid || !verTrackGrid) {
                std::cout << "[ERROR] No track grid! Exiting...\n";
                std::exit(1);
        }

        int minSpacingX = 0;
        int minSpacingY = 0;
        int initTrackX = 0;
        int initTrackY = 0;
        int minAreaX = 0;
        int minAreaY = 0;
        int minWidthX = 0;
        int minWidthY = 0;
        
        int numTracks = -1;
        verTrackGrid->getGridPatternX(0, initTrackX, numTracks, minSpacingX);
        horTrackGrid->getGridPatternY(0, initTrackY, numTracks, minSpacingY);

        minAreaX =  verLayer->getArea();
        minWidthX = verLayer->getWidth();
        minAreaY =  horLayer->getArea();
        minWidthY = horLayer->getWidth();

        *_core = Core(lowerBound, upperBound, minSpacingX * 2, minSpacingY * 2,
                      initTrackX, initTrackY, minAreaX, minAreaY,
                      minWidthX, minWidthY, databaseUnit);

        if(_verbose) {
                std::cout << "lowerBound: " << lowerBound.getX() << " " << lowerBound.getY() << "\n";
                std::cout << "upperBound: " << upperBound.getX() << " " << upperBound.getY() << "\n";
                std::cout << "minSpacingX: " << minSpacingX << "\n";
                std::cout << "minSpacingY: " << minSpacingY << "\n";
                std::cout << "initTrackX: " << initTrackX << "\n";
                std::cout << "initTrackY: " << initTrackY << "\n";
                std::cout << "minAreaX: " << minAreaX << "\n";
                std::cout << "minAreaY: " << minAreaY << "\n";
                std::cout << "minWidthX: " << minWidthX << "\n";
                std::cout << "minWidthY: " << minWidthY << "\n";
                std::cout << "databaseUnit: " << databaseUnit << "\n";
        }
}

void DBWrapper::initNetlist() {
        ads::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] ads::dbBlock not found! Exiting...\n";
                std::exit(1);
        }

        ads::dbSet<ads::dbBTerm> bterms = block->getBTerms();

        if(bterms.size() == 0) {
                std::cout << "[ERROR] Design without pins. Exiting...\n";
                std::exit(1);
        }

        ads::dbSet<ads::dbBTerm>::iterator btIter;
        
        for(btIter = bterms.begin(); btIter != bterms.end(); ++btIter) {
                ads::dbBTerm* curBTerm = *btIter;
                ads::dbNet* net =  curBTerm->getNet();
                if (!net) {
                        std::cout << "[WARNING] Pin " << curBTerm->getConstName()
                                  << " without net!\n";
                }

                Direction dir = DIR_INOUT;
                switch( curBTerm->getIoType() ) {
                        case ads::dbIoType::INPUT:
                                dir = DIR_IN;
                                break;
                        case ads::dbIoType::OUTPUT:
                                dir = DIR_OUT;
                                break;
                }

                int xPos = 0;
                int yPos = 0;
                curBTerm->getFirstPinLocation( xPos, yPos );
               
                Coordinate bounds(0, 0);
                IOPin ioPin( curBTerm->getConstName(), 
                             Coordinate(xPos, yPos), 
                             dir, bounds, bounds, net->getConstName(),
                             "FIXED" );

                std::vector<InstancePin> instPins;
                ads::dbSet<ads::dbITerm> iterms = net->getITerms();
                ads::dbSet<ads::dbITerm>::iterator iIter;
                for(iIter = iterms.begin(); iIter != iterms.end(); ++iIter) {
                        ads::dbITerm* curITerm = *iIter;
                        ads::dbInst* inst = curITerm->getInst();
                        int instX = 0, instY = 0;
                        inst->getLocation(instX, instY);
                        
                        instPins.push_back(InstancePin(inst->getConstName(), 
                                                       Coordinate(instX, instY)));
                }

                _netlist->addIONet(ioPin, instPins);
        }
}

void DBWrapper::commitIOPlacementToDB(std::vector<IOPin>& assignment) {
        ads::dbBlock* block = _chip->getBlock();
        if (!block) {
                std::cout << "[ERROR] ads::dbBlock not found! Exiting...\n";
                std::exit(1);
        }
       
        ads::dbTech* tech = _db->getTech();
        if (!tech) {
                std::cout << "[ERROR] ads::dbTech not initialized! Exiting...\n";
                std::exit(1);
        }
 
        int horLayerIdx = _parms->getHorizontalMetalLayer();
        int verLayerIdx = _parms->getVerticalMetalLayer();

        ads::dbTechLayer* horLayer = tech->findRoutingLayer(horLayerIdx);
        if (!horLayer) {
                std::cout << "[ERROR] Layer" << horLayerIdx << " not found! Exiting...\n";
                std::exit(1);
        }

        ads::dbTechLayer* verLayer = tech->findRoutingLayer(verLayerIdx);
        if (!horLayer) {
                std::cout << "[ERROR] Layer" << verLayerIdx << " not found! Exiting...\n";
                std::exit(1);
        }


        for (IOPin& pin: assignment) {
                ads::dbBTerm* bterm = block->findBTerm(pin.getName().c_str());
                ads::dbSet<ads::dbBPin> bpins = bterm->getBPins();
                ads::dbSet<ads::dbBPin>::iterator bpinIter;
                std::vector<ads::dbBPin*> allBPins;
                for(bpinIter = bpins.begin(); bpinIter != bpins.end(); ++bpinIter) {
                        ads::dbBPin* curBPin = *bpinIter;
                        allBPins.push_back(curBPin);
                } 
                               
                for(ads::dbBPin* bpin: allBPins) {
                        ads::dbBPin::destroy(bpin);
                } 
                       
                Coordinate lowerBound = pin.getLowerBound();
                Coordinate upperBound = pin.getUpperBound();

                ads::dbBPin* bpin = ads::dbBPin::create(bterm);
                
                int size = upperBound.getX() - lowerBound.getX();

                int xMin = lowerBound.getX();
                int yMin = lowerBound.getY();
                int xMax = upperBound.getX();
                int yMax = upperBound.getY();
                ads::dbTechLayer* layer = verLayer;
                if (pin.getOrientation() == Orientation::ORIENT_EAST ||
                    pin.getOrientation() == Orientation::ORIENT_WEST) {
                        layer = horLayer;
                }
                     
                //std::cout << pin.getX() << "\n";
                ads::dbBox::create(bpin, layer, xMin, yMin, xMax, yMax);
                bpin->setPlacementStatus(ads::dbPlacementStatus::PLACED);
        };
}

void DBWrapper::writeDEF() {
        ads::dbBlock* block = _chip->getBlock();
        
        ads::defout writer;
        
        std::string defFileName = _parms->getOutputDefFile();
        
        writer.setVersion( ads::defout::DEF_5_6 );
        writer.writeBlock( block, defFileName.c_str() );
}
