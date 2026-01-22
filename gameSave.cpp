//gameSave.cpp

#include "WorldManager.h"
#include "gameSave.h"
#include "Object.h"
#include "EventKeyboard.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include "bomb.h"
#include "Fruit.h"

gameSave::gameSave(){
    //register for keyboard events
    registerInterest(df::KEYBOARD_EVENT);

}

int gameSave::eventHandler(const df:: Event *p_e){
    //check df::Keyboard event
    if (p_e->getType() == df::KEYBOARD_EVENT){
    //cast to eventkeyboars
        const df::EventKeyboard *p_k = dynamic_cast <const df::EventKeyboard *> (p_e);
    //check for S and L
        if(p_k->getKey() ==df::Keyboard::S){
            saveGame();
            return 1;
        }
        if(p_k->getKey() ==df::Keyboard::L){
            loadGame();
            return 1;
        }
    }
    return 0;
}

void gameSave::saveGame(){
    //finding all objects
    df::ObjectList all_objects = WM.getAllObjects();
    //storing in iterator
    df::ObjectListIterator list_object(&all_objects);
    //counting all objects
    int count = 0;
    // Create stringstream to hold serialized data.
    std::stringstream ss;
    unsigned int mask = UINT_MAX; // all bits set
    //loop to get total amount of objects for save_count
    for(list_object.first(); !list_object.isDone(); list_object.next()){
        df:: Object *p_o = list_object.currentObject();
        //if it is not a fader or a gameSave Object
        if(p_o != this && p_o -> getType() != "Fader"){
            count++;
        }
    }
    
    //save count of object to the stream
    ss << count << std::endl;
    //open file
    std::ofstream outFile("save.txt");
    
    //loop again for the serialize
    for(list_object.first(); !list_object.isDone(); list_object.next()){
        df:: Object *p_o = list_object.currentObject();
        if(p_o != this && p_o -> getType() != "Fader"){
            //get type, save for the object to be seralized
            ss << p_o->getType() << std::endl;
            // Serialize the Object.
            p_o -> serialize(&ss, mask);
        }
    }
    //write to the stream
    std::string str = ss.str();            
    outFile.write(str.c_str(), str.size());
    //close file
    outFile.close();
}

void gameSave::loadGame(){
    //finding all objects
    df::ObjectList all_objects = WM.getAllObjects();
    //storing in iterator
    df::ObjectListIterator list_object(&all_objects);
    for(list_object.first(); !list_object.isDone(); list_object.next()){
        df:: Object *p_o = list_object.currentObject();
        //deleting all objects
        if(p_o != this){
            WM.markForDelete(p_o);
        }
    }
    // Open file for reading.
    std::ifstream inFile("save.txt");
    std::stringstream ss;
    // Read entire file into stream.
    ss << inFile.rdbuf();

    // Close file.
    inFile.close();

    // Extract number from stream.
    int count; //total number of objects
    ss >> count;

    for(int i = 0; i < count; i++){
        std::string type;
        ss >> type; // extract the type for the object
        df::Object *p_o = nullptr;
        //based on type, create the object
        if(type == "bomb"){
            p_o = new Bomb();
        }
        else {
            p_o = new Fruit(type);
        }
        if(p_o){
            unsigned int mask = UINT_MAX; // all bits set
            //deserialize stream on the object
            p_o->deserialize(&ss, &mask);
        }

    }
}