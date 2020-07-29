#include "room.h"
#include "robot.h"

void setEmptyTilesInteresting(s_room* room) {
  for(int i=0; i<room->sizeX; i++)
    for(int j=0; j<room->sizeY; j++) {
      if(room->nodes[j][i].robotVision == TILE_INTERESTING)
        room->nodes[j][i].robotVision = TILE_EMPTY;

      if(room->nodes[j][i].robotVision == TILE_EMPTY)
        room->nodes[j][i].robotVision = TILE_POTENTIAL_FIRE;
    }

  //Check if 3*3 interesting zone exist, set the interesting point in the center.
  for(int i=2; i<room->sizeX-2; i++)
    for(int j=2; j<room->sizeY-2; j++) {
      int pointFound = 1;

      for(int k=-1; k<=1; k++)
        for(int l=-1; l<=1; l++)
          if(room->nodes[j+k][i+l].robotVision != TILE_POTENTIAL_FIRE)
            pointFound = 0;

      if(pointFound) {
        for(int k=-1; k<=1; k++)
          for(int l=-1; l<=1; l++)
            room->nodes[j+k][i+l].robotVision = TILE_EMPTY;

        room->nodes[j][i].robotVision = TILE_INTERESTING;
      }
    }

  //Check if 3*2 interesting zones exist, set the interesting point in the center;
  for(int i=2; i<room->sizeX-2; i++)
    for(int j=1; j<room->sizeY-1; j++) {
      int pointFound = 1;

      for(int k=0; k<=1; k++)
        for(int l=-1; l<=1; l++)
          if(room->nodes[j+k][i+l].robotVision != TILE_POTENTIAL_FIRE)
            pointFound=0;

      if(pointFound) {
        for(int k=0; k<=1; k++)
          for(int l=-1; l<=1; l++)
            room->nodes[j+k][i+l].robotVision = TILE_EMPTY;

        room->nodes[j][i].robotVision = TILE_INTERESTING;
      }
    }

  for(int i=1; i<room->sizeX-1; i++)
    for(int j=2; j<room->sizeY-2; j++) {
      int pointFound = 1;

      for(int k=-1; k<=1; k++)
        for(int l=0; l<=1; l++)
          if(room->nodes[j+k][i+l].robotVision != TILE_POTENTIAL_FIRE)
            pointFound=0;

      if(pointFound) {
        for(int k=-1; k<=1; k++)
          for(int l=0; l<=1; l++)
            room->nodes[j+k][i+l].robotVision = TILE_EMPTY;

        room->nodes[j][i].robotVision = TILE_INTERESTING;
      }
    }

  //Check if 2*2 interesting zones exist, set the interesting point in the center;
  for(int i=1; i<room->sizeX-1; i++)
    for(int j=1; j<room->sizeY-1; j++) {
      int pointFound = 1;

      for(int k=0; k<=1; k++)
        for(int l=0; l<=1; l++)
          if(room->nodes[j+k][i+l].robotVision != TILE_POTENTIAL_FIRE)
            pointFound = 0;

      if(pointFound) {
        for(int k=0; k<=1; k++)
          for(int l=0; l<=1; l++)
            room->nodes[j+k][i+l].robotVision = TILE_EMPTY;

        room->nodes[j][i].robotVision = TILE_INTERESTING;
      }
    }

  //Check if 5*1 or 3*1 interesting zones exist, set the interesting point in the center;
  for(int i=2; i<room->sizeX-2; i++)
    for(int j=1; j<room->sizeY-1; j++) {
      int pointFound = 1;

      for(int k=-2; k<=2; k++)
        if(room->nodes[j][i+k].robotVision != TILE_POTENTIAL_FIRE)
          pointFound = 0;

      if(pointFound) {
        for(int k=-2; k<=2; k++)
          room->nodes[j][i+k].robotVision = TILE_EMPTY;

        room->nodes[j][i].robotVision = TILE_INTERESTING;
      }
    }

  for(int i=1; i<room->sizeX-1; i++)
    for(int j=1; j<room->sizeY-1; j++) {
      int pointFound = 1;

      for(int k=-1; k<=1; k++)
        if(room->nodes[j][i+k].robotVision != TILE_POTENTIAL_FIRE)
          pointFound = 0;

      if(pointFound) {
		for(int k=-1; k<=1; k++)
          room->nodes[j][i+k].robotVision = TILE_EMPTY;

        room->nodes[j][i].robotVision = TILE_INTERESTING;
      }
  }


  for(int i=1; i<room->sizeX-1; i++)
    for(int j=2; j<room->sizeY-2; j++) {
      int pointFound = 1;

      for(int k=-2; k<=2; k++)
        if(room->nodes[j+k][i].robotVision != TILE_POTENTIAL_FIRE)
          pointFound = 0;

      if(pointFound) {
        for(int k=-2; k<=2; k++)
          room->nodes[j+k][i].robotVision = TILE_EMPTY;

        room->nodes[j][i].robotVision = TILE_INTERESTING;
      }
    }

  for(int i=1; i<room->sizeX-1; i++)
    for(int j=1; j<room->sizeY-1; j++) {
      int pointFound = 1;

      for(int k=-1; k<=1; k++)
        if(room->nodes[j+k][i].robotVision != TILE_POTENTIAL_FIRE)
          pointFound = 0;

      if(pointFound) {
        for(int k=-1; k<=1; k++)
          room->nodes[j+k][i].robotVision = TILE_EMPTY;

        room->nodes[j][i].robotVision = TILE_INTERESTING;
      }
    }


  //Check if 2*2 with one . missing interesting zones exist, set the interesting point in the center;
  for(int i=0; i<room->sizeX-1; i++)
    for(int j=0; j<room->sizeY-1; j++) {
      int pointFound = 2;

      for(int k=0; k<=1; k++)
        for(int l=0; l<=1; l++)
          if(room->nodes[j+k][i+l].robotVision != TILE_POTENTIAL_FIRE)
            pointFound--;

      if(pointFound > 0) {
        pointFound = 0;

        for(int k=0; k<=1; k++)
          for(int l=0; l<=1; l++) {
            if(room->nodes[j+k][i+l].robotVision == TILE_POTENTIAL_FIRE && !pointFound) {
              room->nodes[j+k][i+l].robotVision = TILE_INTERESTING;
              pointFound=1;
            }
            else if(room->nodes[j+k][i+l].robotVision == TILE_POTENTIAL_FIRE)
              room->nodes[j+k][i+l].robotVision = TILE_EMPTY;
          }
      }
    }

  for(int i=0; i<room->sizeX-1; i++)
    for(int j=0; j<room->sizeY-1; j++) {
      int pointFound = 3;

      for(int k=0; k<=1; k++)
        for(int l=0; l<=1; l++)
          if(room->nodes[j+k][i+l].robotVision != TILE_POTENTIAL_FIRE)
            pointFound--;

      if(pointFound > 0) {
        pointFound = 0;

        for(int k=0; k<=1; k++)
          for(int l=0; l<=1; l++) {
            if(room->nodes[j+k][i+l].robotVision == TILE_POTENTIAL_FIRE && !pointFound) {
              room->nodes[j+k][i+l].robotVision = TILE_INTERESTING;
              pointFound=1;
            }
            else if(room->nodes[j+k][i+l].robotVision == TILE_POTENTIAL_FIRE)
              room->nodes[j+k][i+l].robotVision = TILE_EMPTY;
          }
      }
    }


  for(int i=0; i<room->sizeX; i++)
    for(int j=0; j<room->sizeY; j++)
      if(room->nodes[j][i].robotVision == TILE_POTENTIAL_FIRE)
        room->nodes[j][i].robotVision = TILE_INTERESTING;

  //Voir si ya aussi moyen d'optimiser la recherche de pattern : genre break si un mur ?
}

vector nextNodePath(s_room* room) {
  if(room->robot.hasExtinguisher == 0)
    printf("[WARNING] The robot is searching the fire but doesn't have the extinguisher.\n");

  vector interestingPoints;
  vector_init(&interestingPoints);

  for(int i=0; i<room->sizeX; i++)
    for(int j=0; j<room->sizeY; j++)
      if(room->nodes[j][i].robotVision == TILE_INTERESTING)
        vector_add(&interestingPoints, &(room->nodes[j][i]));

  int* distances = malloc(vector_total(&interestingPoints)*sizeof(int));
  for(int i=0; i<vector_total(&interestingPoints); i++) {
    s_node* node = (s_node*)vector_get(&interestingPoints, i);
    distances[i] = getDistance(room, node->pos);
  }

  vector bestNodes;
  vector_init(&bestNodes);
  vector_add(&bestNodes, vector_get(&interestingPoints, 0));

  int minDist = distances[0];
  for(int i=1; i<vector_total(&interestingPoints); i++)
    if(distances[i] < minDist) {
      vector_free(&bestNodes);
      vector_init(&bestNodes);
      vector_add(&bestNodes, vector_get(&interestingPoints, i));

      minDist = distances[i];
    } else if(distances[i] == minDist) {
      vector_add(&bestNodes, vector_get(&interestingPoints, i));
    }

  s_node* node = (s_node*)vector_get(&bestNodes, 0);

  free(distances);
  vector_free(&interestingPoints);

  room->robot.status = STATUS_SEARCH_FIRE;
  return getBestPath(room, room->robot.pos, node->pos);
}
