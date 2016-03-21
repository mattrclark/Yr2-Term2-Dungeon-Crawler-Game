#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    music.load("music/crawler2.mp3");
    music.setLoop(true);
    music.play();
    font = new ofTrueTypeFont();
    font->load(OF_TTF_SANS, 18);
    loadImages();
    lvl =1;
    player = new Player(ofVec2f(300,300));
    floor = new Floor(lvl);
    currentRoom = floor->getRoom();
    grid = floor->getGrid();
    size = 7;
    shootLeft = false;
    shootRight = false;
    shootUp = false;
    shootDown = false;
    nextFloor = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    player->move();
    
    if(shootLeft){
        if(player->fire()){
            Projectile* new_bullet = new Projectile(player->getPos(), player->getShotSpeed(), player->getDamage());
            bullets.push_back(new_bullet);
            new_bullet->moveLeft();
            new_bullet = nullptr;
        }
    }
    if(shootRight){
        if(player->fire()){
            Projectile* new_bullet = new Projectile(player->getPos(), player->getShotSpeed(),player->getDamage());
            bullets.push_back(new_bullet);
            new_bullet->moveRight();
            new_bullet = nullptr;
        }
    }
    if(shootUp){
        if(player->fire()){
            Projectile* new_bullet = new Projectile(player->getPos(), player->getShotSpeed(),player->getDamage());
            bullets.push_back(new_bullet);
            new_bullet->moveUp();
            new_bullet = nullptr;
        }
    }
    if(shootDown){
        if(player->fire()){
            Projectile* new_bullet = new Projectile(player->getPos(), player->getShotSpeed(),player->getDamage());
            bullets.push_back(new_bullet);
            new_bullet->moveDown();
            new_bullet = nullptr;
        }
    }
    
    
    vector<Door*> doors = currentRoom->getDoors();
    vector<Enemy*> enemies = currentRoom->getEnemies();
    vector<Pickup*> pickups = currentRoom->getPickups();
    vector<Tile*> tiles = currentRoom->getTiles();
    for(auto _proj: bullets){
        _proj->move();
        if(_proj->edgeDetect()){
            _proj->die();
        }
    }
    for(auto &_ene: enemies){
        _ene->moveNextPattern();
        _ene->movePattern();
        _ene->move();
        if(_ene->collide(player)){
            _ene->collisionResponse(player);
        }
        for(auto _proj: bullets){
            if(_ene->collide(_proj)){
                _ene->collisionResponse(_proj);
                _proj->collisionResponse(_ene);
            }
        }
    }
    
    for(auto &_pu: pickups){
        if(_pu->collide(player)){
            _pu->collisionResponse(player);
        }
    }
    
    if(currentRoom->checkEmpty()){
        for(auto &_door: doors){
            if(_door->collideLeft(player)){
                killBullets();
                player->setPos(ofVec2f(75,375));
                floor->moveRoom(GridPos(0,1));
                currentRoom = floor->getRoom();
                cout<<"left"<<endl;
            }
            if(_door->collideRight(player)){
                killBullets();
                player->setPos(ofVec2f(725,375));
                floor->moveRoom(GridPos(0,-1));
                currentRoom = floor->getRoom();
                cout<<"right"<<endl;
            }
            if(_door->collideTop(player)){
                killBullets();
                player->setPos(ofVec2f(400,175));
                floor->moveRoom(GridPos(1,0));
                currentRoom = floor->getRoom();
                cout<<"top"<<endl;
            }
            if(_door->collideBottom(player)){
                killBullets();
                player->setPos(ofVec2f(400,575));
                floor->moveRoom(GridPos(-1,0));
                currentRoom = floor->getRoom();
                cout<<"bottom"<<endl;
            }
        }
        
        for(auto &_tile: tiles){
            Trapdoor* _trapdoor = dynamic_cast<Trapdoor*>(_tile);
            if(_trapdoor){
                cout<<"trap"<<endl;
                if(_trapdoor->collide(player)){
                    cout<<"trapCollide"<<endl;
                    nextFloor = true;
                }
            }
        }
    }
    
    checkDead();
    currentRoom->checkDead();
    
    //Move to next floor
    if(nextFloor){
        lvl++;
        delete floor; // Delete floor before creating new one to avoid memory leak. Avg memory = ~20MB
        floor = new Floor(lvl);
        currentRoom = floor->getRoom();
        grid = floor->getGrid();
        nextFloor = false;
    }
//    cout<<currentRoom->getFloorPos().x<<" "<<currentRoom->getFloorPos().y<<endl;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(120, 82, 53);
    
    //Display all game objects
    currentRoom->display();
    for(auto _proj: bullets){
        _proj->display();
    }
    player->display();
    
    
    //Draw all player information
    //Background - Black
    ofPushStyle();
        ofSetColor(0);
        ofDrawRectangle(0, 0, ofGetWidth(), 100);
    ofPopStyle();
    
    //Player stats
    ofPushStyle();
        ofSetColor(255);
        font->drawString("Health:", 150, 40);
        font->drawString("Damage: "+to_string(player->getDamage()), 450, 20);
    font->drawString("Fire Rate: "+to_string(100/player->getFireRate()), 450, 40);
    font->drawString("Speed: "+to_string(player->getMaxSpeed()), 450, 60);
    font->drawString("Shot Speed: "+to_string(player->getShotSpeed()), 450, 80);
    font->drawString("Floor: "+to_string(lvl), 150, 70);
    ofPopStyle();
    
    //Player Health bar
    ofPushMatrix();
        ofTranslate(225, 20);
        ofPushStyle();
        ofSetColor(255, 0, 0);
            ofDrawRectangle(0, 0, 200, 20);
        ofPopStyle();
    
        ofPushStyle();
            ofSetColor(0, 255, 0);
            ofDrawRectangle(0, 0, (200*player->getHealth()/player->getMaxHealth()), 20);
        ofPopStyle();
    ofPopMatrix();
    
    //Floor map
    glPushMatrix();
    glTranslated(10, 5, 0);
    for(int i=0; i<grid[0].size(); i++){
        for(int j=0; j<grid.size(); j++){
            switch (grid[j][i]) {
                case 0:
                    break;
                default:
                    ofPushStyle();
                    ofSetColor(0);
                    ofNoFill();
                    ofDrawRectangle(i*size, j*size, size, size);
                    ofPopStyle();
                    
                    ofPushStyle();
                    ofSetColor(255);
                    ofFill();
                    ofDrawRectangle(i*size, j*size, size, size);
                    ofPopStyle();
                    break;
            }
        }
    }
    glPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key=='a'){
        player->moveLeft();
    }
    if(key=='d'){
        player->moveRight();
    }
    if(key=='w'){
        player->moveUp();
    }
    if(key=='s'){
        player->moveDown();
    }
    
    //Only lets the player shoot in 1 direction at any 1 time
    if(!shootLeft && !shootRight && !shootUp && !shootDown){
        if(key==OF_KEY_LEFT){
            shootLeft = true;
        }
        if(key==OF_KEY_RIGHT){
            shootRight = true;
        }
        if(key==OF_KEY_UP){
            shootUp = true;
        }
        if(key==OF_KEY_DOWN){
            shootDown = true;
        }
    }
    
    if(key=='.'){
        lvl++;
        delete floor; // Delete floor before creating new one to avoid memory leak. Avg memory = ~20MB
        floor = new Floor(lvl);
        currentRoom = floor->getRoom();
        grid = floor->getGrid();
    }
    if(key==','){
        lvl--;
        delete floor; // Delete floor before creating new one to avoid memory leak. Avg memory = ~20MB
        floor = new Floor(lvl);
        currentRoom = floor->getRoom();
        grid = floor->getGrid();
    }
    if(key=='l'){
        currentRoom->checkDead();
    }
//    cout<<lvl<<endl;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='a'){
        player->stopLeft();
    }
    if(key=='d'){
        player->stopRight();
    }
    if(key=='w'){
        player->stopUp();
    }
    if(key=='s'){
        player->stopDown();
    }
    if(key==OF_KEY_LEFT){
        shootLeft = false;
    }
    if(key==OF_KEY_RIGHT){
        shootRight = false;
    }
    if(key==OF_KEY_UP){
        shootUp = false;
    }
    if(key==OF_KEY_DOWN){
        shootDown = false;
    }

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

 /*
 * Loads all images in data folder
  Reference: Marco Gillies. Image Manager Program
  https://github.com/marcogillies/ImageManager
 */
 void ofApp::loadImages(){
 
     // clear all of the object
     // and the image manager
     // (deletes all of the pointers)
     ImageManager::get().reset();
     
     // get all of the .png images in the
     // directory
     ofDirectory dir(ofToDataPath("allImages"));
     //only show png files
     dir.allowExt("png");
     //populate the directory object
     dir.listDir();
     
     //go through and add each image to the image manager
     for(int i = 0; i < dir.size(); i++){
         cout<<dir.getPath(i)<<endl;
     ImageManager::get().add(dir.getPath(i));
     }
 };

void ofApp::checkDead(){
    //Reference: Marco Gilles - ShooterInheritence
    // remove all dead objects
    // only do this after the two loops to avoid
    // invalidated iterators
    auto it = std::remove_if(bullets.begin(), bullets.end(),
                             [](Projectile *_proj){
                                 return !_proj->isAlive();
                             });
    for(auto _proj: bullets){
        if(!_proj->isAlive()){
            delete _proj;
            _proj = nullptr;
        }
    }
    bullets.erase(it, bullets.end());
}

void ofApp::killBullets(){
    for(auto _proj: bullets){
        _proj->die();
    }
}










//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
