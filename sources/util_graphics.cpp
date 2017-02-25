#ifndef UTIL_GRAPHICS
#define UTIL_GRAPHICS

struct Camera{
    mat4 frustumMatrix;
    v2 facing;
    v3 position;
};



struct Image{
    Uint32 width;
    Uint32 height;
    Uint32 * pixeldata;
    
};



void setCameraDefaultPerspective(v2 viewport, Camera * currentCamera){
    float32 fzNear = 0.05;
    float32 fzFar = 100.0f;
    float32 fov = 90.0f;
    float32 focalLength = 1.0f / tan((degToRad(fov) / 2.0f));
    currentCamera->frustumMatrix.c[0] = focalLength;
    currentCamera->frustumMatrix.c[5] = focalLength / (viewport.y /viewport.x);
    currentCamera->frustumMatrix.c[10] = -1.0f*(fzFar + fzNear) / (fzFar - fzNear);
    currentCamera->frustumMatrix.c[14] = (-2.0f * fzFar * fzNear) / (fzFar - fzNear);
    currentCamera->frustumMatrix.c[11] = -1.0f;
}

/*
struct meshFaceElement{
  v3 vertex;
  v3 normal;
  v2 uv;
  char identification[20];
};

//.obj limited size!!!!!
//not supporting groups
struct meshFile{
    //mirrors array buffer in opengl //vertices, normals
    float32 arrayBuffer[1000000];
    Uint32 arrayBufferCount;
    //mirriors array elements buffer, order {index, index, index[, index]^j}^n
    Uint32 indexBuffer[500000];
    Uint32 indexBufferCount;
    char materialNames[MATERIALS_AMOUNT][255];
    Uint32 materialCount;


    meshFaceElement meshElements[250000];
    Uint32 meshFaceElementCount;

    char materialLib[20];

    v3 normalsTemp[110000];
    Uint32 normalsTempCount;
    v3 verticesTemp[120000];
    Uint32 verticesTempCount;
    v2 textureCoordsTemp[120000];
    Uint32 textureCoordsTempCount;

    Uint32 normalsOffset;
    Uint32 textureCoordsOffset;

    Uint32 contentBytes;
    char fileContents[MEGABYTE(20)];
};

struct materialFile{
  Uint32 contentBytes;
  char fileContents[MEGABYTE(10)];
};

//TODO:: load textures per unique file name and link correctly to avoid duplication
void loadTexture(const char * filename, meshFile * meshData, const Uint8 samplerIndex, gl_material * targetMaterial, void * buffer){
  char finalFilename[256] = "../sources/models/";
  SDL_Surface * texture = IMG_Load(strcat(finalFilename, filename));
  assert(texture);

  Uint32 pixelAmount = texture->w * texture->h;
  assert((pixelAmount * 4) < MAXIMUM_TEXTURE_SIZE);

  unsigned char * textureBufer = (unsigned char * ) buffer;
  Uint32 textureBufferSize = 0;

//surfaces are topleft indexed, we need bottomleft
  for(int i = 0; i < pixelAmount; i++){
      Uint32 row = i / texture->w;
      Uint32 colorOffset = (texture->h-1-row)*texture->w*4 + (i % texture->w)*4;
      Uint32 composedColor = 0;
      for(int element = 0; element < texture->format->BytesPerPixel; element++){
           composedColor = composedColor | (((Uint8 *) texture->pixels)[i*texture->format->BytesPerPixel+element] << (8*element));
      }
      if(texture->format->Rmask | texture->format->Gmask | texture->format->Bmask | texture->format->Amask){
        Uint32 redComponent = composedColor & texture->format->Rmask;
        Uint32 greenComponent = composedColor & texture->format->Gmask;
        Uint32 blueComponent = composedColor & texture->format->Bmask;
        Uint32 alphaComponent = composedColor & texture->format->Amask;
        while(redComponent > 255) redComponent = redComponent >> 8;
        while(greenComponent > 255) greenComponent = greenComponent >> 8;
        while(blueComponent > 255) blueComponent = blueComponent >> 8;
        while(alphaComponent > 255) alphaComponent = alphaComponent >> 8;
        textureBufer[colorOffset] = redComponent;
        textureBufer[colorOffset + 1] = greenComponent;
        textureBufer[colorOffset + 2] = blueComponent;
        textureBufer[colorOffset + 3] = alphaComponent;
        textureBufferSize += 4;
      }else{
        //pallete is used
        SDL_Color finalColor = texture->format->palette->colors[composedColor];
        textureBufer[colorOffset] = finalColor.r;
        textureBufer[colorOffset + 1] = finalColor.g;
        textureBufer[colorOffset + 2] = finalColor.b;
        textureBufer[colorOffset + 3] = finalColor.a;
        textureBufferSize += 4;
      }

  }
  //is this needed?
   glActiveTexture(GL_TEXTURE0 + samplerIndex);
   glGenTextures(1, &targetMaterial->texturesData[samplerIndex].texture2Dbuffer);
   glBindTexture(GL_TEXTURE_2D, targetMaterial->texturesData[samplerIndex].texture2Dbuffer);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texture->w, texture->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBufer);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glGenSamplers(1, &targetMaterial->texturesData[samplerIndex].sampler2D);
   glBindSampler(samplerIndex, targetMaterial->texturesData[samplerIndex].sampler2D);
   glSamplerParameteri(targetMaterial->texturesData[samplerIndex].sampler2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glSamplerParameteri(targetMaterial->texturesData[samplerIndex].sampler2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glSamplerParameteri(targetMaterial->texturesData[samplerIndex].sampler2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

}
//TODO: parsing also whils ignoring arbitrary whitespaces

void loadMaterial(const char * filename, materialFile * memory, meshFile * meshData, gl_data * level, Uint8 * globalMaterialCount){
  materialFile & material = *memory;
  meshFile & mesh = *meshData;


  char finalFilename[256] = "../sources/models/";
  SDL_RWops * materialFile = SDL_RWFromFile(strcat(finalFilename, filename), "r");
  material.contentBytes = materialFile->size(materialFile);
  assert(material.contentBytes < MEGABYTE(10));
  materialFile->read(materialFile, material.fileContents, material.contentBytes, 1);
  materialFile->close(materialFile);

  Uint32 readBytes = 0;
  mesh.materialCount = 0;

  gl_material * targetMaterial;


  //Ke, Ni & d ignored
          while(readBytes < material.contentBytes){
            char row[50000] = {};
            Uint16 rowLength = 0;
            Uint16 index = 0;
            while((row[rowLength++] = material.fileContents[readBytes+index++]) != '\n');
            readBytes += rowLength;
            char type[10];
            sscanf(row, "%10s", type);
            if(!strcmp(type, "newmtl")){
                sscanf(row + 6, "%s\n", mesh.materialNames[mesh.materialCount]);
                targetMaterial = &level->materials[*globalMaterialCount];
                (*globalMaterialCount)++;
                mesh.materialCount++;
                level->materials[*globalMaterialCount-1] = {};
                assert(*globalMaterialCount <= MATERIALS_AMOUNT);
            }
            if(!strcmp(type, "Kd")){

              //diffuse color
              v4 color = {0,0,0,1};
              sscanf(row + 2, "%f %f %f %f", &color.r, &color.g, &color.b, &color.a);
              targetMaterial->diffuseColor = color;

            }
            if(!strcmp(type, "Ka")){
              //ambientColor
              v4 color = {0,0,0,1};
              sscanf(row + 2, "%f %f %f %f", &color.r, &color.g, &color.b, &color.a);
              targetMaterial->ambientColor = color;
            }
            if(!strcmp(type, "Ks")){
              //specular color;
              v4 color = {0,0,0,1};
              sscanf(row + 2, "%f %f %f %f", &color.r, &color.g, &color.b, &color.a);
              targetMaterial->specularColor = color;
            }
            if(!strcmp(type, "Ns")){
              //shinines factor;
              sscanf(row + 2, "%f", &targetMaterial->shininessFactor);
            }
            if(!strcmp(type, "illum")){
              //illumination type
              Sint32 illum;
              sscanf(row + 5, "%d", &illum);
              targetMaterial->illumLevel = illum;
            }
            if(!strcmp(type, "map_Kd")){
              //diffuse texture
              char filename[256];
              sscanf(row + 6, "%s\n", filename);
              void * buffer = (void *)(memory + 1);
              assert(sizeof(meshFile) + sizeof(materialFile) + MAXIMUM_TEXTURE_SIZE < GAME_MEMORY_TEMP_SIZE);
              loadTexture(filename, meshData, DIFFUSE_TEXTURE, targetMaterial, buffer);
              targetMaterial->hasDiffuseTexture = true;
            }
            if(!strcmp(type, "map_Bump")){
              //normal map texture
              char filename[256];
              sscanf(row + 8, "%s\n", filename);
              void * buffer = (void *)(memory + 1);
              assert(sizeof(meshFile) + sizeof(modelDatabase) + sizeof(materialFile) + MAXIMUM_TEXTURE_SIZE < GAME_MEMORY_TEMP_SIZE);
              loadTexture(filename, meshData, NORMAL_TEXTURE, targetMaterial, buffer);
              targetMaterial->hasNormalTexture = true;
            }
          }


}

//ORDER IN THE .obj file matters and it might change with upcoming extensions, so keep an eye on correct order
//TODO: group VAOS by material properly, to minimalize vaos amount...
//TODO: BAKE SCALE INTO vertex data, SO THERE IS ONE LESS TRANSFORMATION
void loadMeshes(gl_data * level, modelDatabase * gameModelDatabase){
    Uint8 materialCount = 0;
    //THE MODEL INDEX MUST BE SAME AS IN DATABASE !!! maybe more robust solution ? initGameWorld uses these indexes
    for(int currentModelIndex = 0; currentModelIndex < arraySize(gameModelDatabase->models); currentModelIndex++){
    gl_object * currentMesh = &level->meshes[currentModelIndex];
    //game model database is at the begginging of the temp memory buffer
    meshFile & mesh = *((meshFile *) (gameModelDatabase + 1));

    mesh.materialLib[0] = '\0';
    assert(sizeof(mesh) + sizeof(modelDatabase) < GAME_MEMORY_TEMP_SIZE);
    //static file for now, not expecting it to have failure format

    char finalFilename[256] = "../sources/models/";
    SDL_RWops * meshFile = SDL_RWFromFile(strcat(finalFilename, gameModelDatabase->models[currentModelIndex].filename), "r");
    mesh.contentBytes = meshFile->size(meshFile);
  	assert(mesh.contentBytes < MEGABYTE(20));
  	meshFile->read(meshFile, mesh.fileContents, mesh.contentBytes, 1);
  	meshFile->close(meshFile);
    v4 vector;

    Uint32 readBytes = 0;



    currentMesh->modelScale.c[0] = gameModelDatabase->models[currentModelIndex].predefinedScale.x;
    currentMesh->modelScale.c[5] = gameModelDatabase->models[currentModelIndex].predefinedScale.y;
    currentMesh->modelScale.c[10] = gameModelDatabase->models[currentModelIndex].predefinedScale.z;
    currentMesh->modelScale.c[15] = 1.0f;

    currentMesh->modelScaleNormal.c[0] = 1.0f/currentMesh->modelScale.c[0];
    currentMesh->modelScaleNormal.c[5] = 1.0f/currentMesh->modelScale.c[5];
    currentMesh->modelScaleNormal.c[10] = 1.0f/currentMesh->modelScale.c[10];
    currentMesh->modelScaleNormal.c[15] = 1.0f;

    //normals and vertices are shared for all objects within file
    mesh.normalsTempCount = 0;
    mesh.verticesTempCount = 0;
    mesh.textureCoordsTempCount = 0;
    mesh.meshFaceElementCount = 0;
    mesh.arrayBufferCount = 0;
    mesh.normalsOffset = 0;
    mesh.textureCoordsOffset = 0;

    while(readBytes < mesh.contentBytes){

      //outer loop for initalizing new buffers before reading bulk of rows
      mesh.indexBufferCount = 0;
      glGenVertexArrays(1, &currentMesh->vaos[currentMesh->vaosAmount]);
      glBindVertexArray(currentMesh->vaos[currentMesh->vaosAmount]);

      //reading bulk lines, o breaks too
    while(readBytes < mesh.contentBytes){
          char row[1000] = {};
          Uint16 rowLength = 0;
          Uint16 index = 0;
          while((row[rowLength++] = mesh.fileContents[readBytes+index++]) != '\n');
          char type[6];
          sscanf(row, "%6s", type);

          readBytes += rowLength;
          if(!strcmp(type, "mtllib")){

              sscanf(row + 6, "%s", mesh.materialLib);
              materialFile & material = *((materialFile * )(&mesh + 1));
              assert(sizeof(mesh) + sizeof(modelDatabase) + sizeof(material) < GAME_MEMORY_TEMP_SIZE);
              loadMaterial(mesh.materialLib, &material, &mesh, level, &materialCount);

          }
          else if(!strcmp(type, "o")){
            //ignoring o, since it has no functional meaning, usemtl is more important
            //just check, that this is not first object
            /*if(mesh.indexBufferCount != 0){
                break;
            }*/

/*


          }
          else if(!strcmp(type, "usemtl")){

            assert(mesh.materialLib[0] != '\0');
            char materialName[255];
            sscanf(row + 6, "%s\n", materialName);
            for(int i = 0; i < mesh.materialCount; i++){
                if(!strcmp(mesh.materialNames[i], materialName)){
                  currentMesh->materialIndices[currentMesh->vaosAmount] = materialCount - mesh.materialCount + i;
                  break;
                }
                assert(i != mesh.materialCount-1);
            }

            if(currentMesh->vaosAmount > 0){
              if(currentMesh->materialIndices[currentMesh->vaosAmount] != currentMesh->materialIndices[currentMesh->vaosAmount-1]){
                //break only if this is not the same material as previous one
                break;
              }
            }
          }
          else if(!strcmp(type, "v")){
                v3 vector;
                Uint8 amount = sscanf(row+2, "%f %f %f\n", &vector.x, &vector.y, &vector.z);
                assert(amount == 3); //might be nan in the file
                if(mesh.verticesTempCount == 0){
                  gameModelDatabase->models[currentModelIndex].AABB.lowerCorner = gameModelDatabase->models[currentModelIndex].AABB.upperCorner = vector;
                }
                mesh.verticesTemp[mesh.verticesTempCount] = vector;
                mesh.verticesTempCount++;
                //AABB construct
                if(vector.x < gameModelDatabase->models[currentModelIndex].AABB.lowerCorner.x){
                  gameModelDatabase->models[currentModelIndex].AABB.lowerCorner.x = vector.x;
                }else if(vector.x > gameModelDatabase->models[currentModelIndex].AABB.upperCorner.x){
                  gameModelDatabase->models[currentModelIndex].AABB.upperCorner.x = vector.x;
                }

                if(vector.y < gameModelDatabase->models[currentModelIndex].AABB.lowerCorner.y){
                  gameModelDatabase->models[currentModelIndex].AABB.lowerCorner.y = vector.y;
                }else if(vector.y > gameModelDatabase->models[currentModelIndex].AABB.upperCorner.y){
                  gameModelDatabase->models[currentModelIndex].AABB.upperCorner.y = vector.y;
                }

                if(vector.z > gameModelDatabase->models[currentModelIndex].AABB.lowerCorner.z){
                  gameModelDatabase->models[currentModelIndex].AABB.lowerCorner.z = vector.z;
                }else if(vector.z < gameModelDatabase->models[currentModelIndex].AABB.upperCorner.z){
                  gameModelDatabase->models[currentModelIndex].AABB.upperCorner.z = vector.z;
                }

          }
          else if(!strcmp(type, "vt")){
                v2 vector;
                Uint8 amount = sscanf(row+2, "%f %f\n", &vector.x, &vector.y);
                mesh.textureCoordsTemp[mesh.textureCoordsTempCount] = vector;

                mesh.textureCoordsTempCount++;

          }
          else if(!strcmp(type, "vn")){
                v3 normal;
                Uint8 amount = sscanf(row+2, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
                normal = normalize(normal);
                mesh.normalsTemp[mesh.normalsTempCount++] = normal;

          }else if(!strcmp(type, "f")){
            Uint32 vertexIndexes[250] = {};
            Uint32 textureIndexes[250] = {};
            Uint32 normalIndexes[250] = {};
            Uint32 elementCount = 0;
            Uint32 offset = 2;
            while(sscanf(row+offset, "%d/%d/%d", &vertexIndexes[elementCount], &textureIndexes[elementCount] , &normalIndexes[elementCount]) == 3
		    || sscanf(row+offset, "%d//%d", &vertexIndexes[elementCount], &normalIndexes[elementCount]) == 2
		    || sscanf(row+offset, "%d//", &vertexIndexes[elementCount]) == 1){
		//always have vertex, not 0
		assert(vertexIndexes[elementCount] > 0);
		
		
              assert(3 + decimalNumberLength(vertexIndexes[elementCount]) + decimalNumberLength(normalIndexes[elementCount]) + decimalNumberLength(textureIndexes[elementCount]) < 20 ); //20 is the identification
              //1 space, 2x /, decimal lengths
	      //if there is missing coordinate, decimal length is 0
              offset += 3 + decimalNumberLength(vertexIndexes[elementCount]) + ((normalIndexes[elementCount] == 0 ) ? 0 : decimalNumberLength(normalIndexes[elementCount])) + ((textureIndexes[elementCount] == 0 ) ? 0 : decimalNumberLength(textureIndexes[elementCount]));
              elementCount++;
            }
            assert(elementCount < arraySize(vertexIndexes));

                //treat each face element
                for(int elementIndex = 0; elementIndex < elementCount; elementIndex++){
                  meshFaceElement fElement = {};
                  sprintf(fElement.identification, "%d/%d/%d", vertexIndexes[elementIndex], textureIndexes[elementIndex], normalIndexes[elementIndex]);
                  //todo hash table later, now just lineary walk through and fetch existing "pairs"
                  bool found = false;
                  for(int i = 0; i < mesh.meshFaceElementCount; i++){
                    if(!strcmp(mesh.meshElements[i].identification, fElement.identification)){
                        mesh.indexBuffer[mesh.indexBufferCount++] = i;
                        found = true;
                    }
                  }
                  if(!found){
                    mesh.indexBuffer[mesh.indexBufferCount++] = mesh.meshFaceElementCount;
                    fElement.vertex = mesh.verticesTemp[vertexIndexes[elementIndex]-1];
                    fElement.normal = mesh.normalsTemp[normalIndexes[elementIndex]-1];
                    fElement.uv = mesh.textureCoordsTemp[textureIndexes[elementIndex]-1];
                    assert(mesh.verticesTempCount > vertexIndexes[elementIndex]-1);
                    assert(mesh.normalsTempCount > normalIndexes[elementIndex]-1);
		    //not 0 => must exist
                    assert(mesh.textureCoordsTempCount == 0 || mesh.textureCoordsTempCount > textureIndexes[elementIndex]-1);
                    mesh.meshElements[mesh.meshFaceElementCount++] = fElement;
                  }
                  assert(mesh.meshFaceElementCount < arraySize(mesh.meshElements));

                  }

                currentMesh->primitivesIndicesCount[currentMesh->vaosAmount][currentMesh->primitivesAmount[currentMesh->vaosAmount]] = elementCount;
                if(currentMesh->primitivesAmount[currentMesh->vaosAmount] == 0){
                  currentMesh->primitivesOffsets[currentMesh->vaosAmount][currentMesh->primitivesAmount[currentMesh->vaosAmount]] = 0;
                }else{
                  //4 bytes per pointer, might not work on 64bit compile
                  currentMesh->primitivesOffsets[currentMesh->vaosAmount][currentMesh->primitivesAmount[currentMesh->vaosAmount]] = (GLvoid *) (((Uint8 * )currentMesh->primitivesOffsets[currentMesh->vaosAmount][currentMesh->primitivesAmount[currentMesh->vaosAmount]-1]) + currentMesh->primitivesIndicesCount[currentMesh->vaosAmount][currentMesh->primitivesAmount[currentMesh->vaosAmount]-1]*4);
                }
                currentMesh->primitivesAmount[currentMesh->vaosAmount]++;

                assert(currentMesh->primitivesAmount[currentMesh->vaosAmount] < arraySize(currentMesh->primitivesOffsets[currentMesh->vaosAmount]));

          }
    }
      assert(mesh.indexBufferCount < sizeof(mesh.indexBuffer));
        GLuint indicesBuffer;
        glGenBuffers(1, &indicesBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (mesh.indexBufferCount)*4, mesh.indexBuffer, GL_STATIC_DRAW);
        currentMesh->vaosAmount++;
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
//copy stuff to vertex buffer



for(int i = 0; i < mesh.meshFaceElementCount; i++){
  mesh.arrayBuffer[mesh.arrayBufferCount++] = mesh.meshElements[i].vertex.x;
  mesh.arrayBuffer[mesh.arrayBufferCount++] = mesh.meshElements[i].vertex.y;
  mesh.arrayBuffer[mesh.arrayBufferCount++] = mesh.meshElements[i].vertex.z;
}
mesh.normalsOffset = mesh.arrayBufferCount;

for(int i = 0; i < mesh.meshFaceElementCount; i++){
  mesh.arrayBuffer[mesh.arrayBufferCount++] = mesh.meshElements[i].normal.x;
  mesh.arrayBuffer[mesh.arrayBufferCount++] = mesh.meshElements[i].normal.y;
  mesh.arrayBuffer[mesh.arrayBufferCount++] = mesh.meshElements[i].normal.z;
}

mesh.textureCoordsOffset = mesh.arrayBufferCount;
for(int i = 0; i < mesh.meshFaceElementCount; i++){
  mesh.arrayBuffer[mesh.arrayBufferCount++] = mesh.meshElements[i].uv.x;
  mesh.arrayBuffer[mesh.arrayBufferCount++] = mesh.meshElements[i].uv.y;
}

assert(mesh.arrayBufferCount < sizeof(mesh.arrayBuffer));
GLuint vertexBuffer;
glGenBuffers(1, &vertexBuffer);
glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
glBufferData(GL_ARRAY_BUFFER, mesh.arrayBufferCount*4, mesh.arrayBuffer, GL_STATIC_DRAW);

for(int i = 0; i <  currentMesh->vaosAmount; i++){

  glBindVertexArray(currentMesh->vaos[i]);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(level->modelPositionLocation);
    glVertexAttribPointer(level->modelPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(level->vertexNormalLocation);
    glVertexAttribPointer(level->vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *) (mesh.normalsOffset*4));
    glEnableVertexAttribArray(level->textureCoordLocation);
    glVertexAttribPointer(level->textureCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void *) (mesh.textureCoordsOffset*4));
  glBindVertexArray(0);
}

}
}

/*
void initCameras(gl_data * level, world * gameWorld){
  for(int cameraIndex = 0; cameraIndex < arraySize(gameWorld->cameras); cameraIndex++){
    camera * currentCamera = &gameWorld->cameras[cameraIndex];
    setCameraDefaultPerspective(level->viewportSize, currentCamera);
  }
}*/

