#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <GLFW/glfw3.h>
#include <stdio.h>

#include <iostream>
using namespace cv;
using namespace std;


int main() {
  
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  } 

  GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  // window devient la fenêtre sur laquelle on travaille et fait les modifications
  glfwMakeContextCurrent(window);


  // on ouvre la video 
  VideoCapture video;
  video.open("/Users/emilieb/Desktop/POLYTECH/Marseille/5A/RA/input.mp4");
  if (!video.isOpened()) { 
    fprintf(stderr, "ERROR: could not open video\n"); 
    return 1; 
  }

  // checker les fps de la video : 30 fps
  double fps = video.get(CAP_PROP_FPS); 
  std::cout << "Frames per seconds : " << fps << endl;

  std::cout << video.get(CAP_PROP_FRAME_WIDTH) << " de largeur" << endl;
  std::cout << video.get(CAP_PROP_FRAME_HEIGHT) << " de hauteur" << endl;

  // identifiant de la texture
  GLuint textureID;
  // génère un identifiant de texture et le range dans textureID
  glGenTextures(1, &textureID);
  // la texture devient active 
  glBindTexture(GL_TEXTURE_2D, textureID);
  //gère l'échantillonage (plus ou moins de pixel texture que de pixel fenêtre) 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // dis a opengl d'utiliser l'image définie dans glGenTextures et glBindTexture lors du rendu
  glEnable(GL_TEXTURE_2D);
  // pour les videos dont la largeur n'est pas multiple de 4
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  double lastFrameTime = 0;
  double currentTime;

  // tant qu'on ne ferme pas la fenêtre (1 boucle, 1 image)
  while (!glfwWindowShouldClose(window)) {
    // fond de couleur
    //glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    // on efface l'image qui était affiché pour en créer une nouvelle 
    glClear(GL_COLOR_BUFFER_BIT);

    // on récupère le temps écoulé depuis le début 
    currentTime = glfwGetTime();
    double dureeFrame = 1.0/fps;

    // on doit changer d'image
    if (currentTime - lastFrameTime >= dureeFrame) {
      // on stocke l'image dans une matrice
      Mat frame;
      Mat grey;
      Mat blur;
      Mat binary;
      Mat colored;
      video >> frame;
      if (frame.empty()) {
        break;
      }
      //lastFrameTime = currentTime; // on ne fait pas ça car ça ne nous permet pas de rattraper le retard s'il y en a 
      lastFrameTime += dureeFrame;



      // 1. PRETRAITEMENT 
      cv::flip(frame, frame, 0);
      // transforme la frame en niveau de gris pour que ce soit moins lourd 
      cvtColor(frame, grey, COLOR_BGR2GRAY);
      //flou gaussien
      GaussianBlur(grey, blur, cv::Size(9, 9), 0);

      // 2. SEGMENTATION
      // seuil avec otsu : renvoie une image binaire avec seuil choisi automatiquement
      double seuil = threshold(blur, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

      
      // 3. GEOMETRIE
      vector<vector<Point>> contours;
      double maxArea= 0.0;
      int maxAreaIndex;
      // trouve tous les contours des zones blanches détectées par otsu
      findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
      // on garde la zone qui a la plus grande aire (feuille A4)
      for (int i =0; i<contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > maxArea) {
          maxArea = area;
          maxAreaIndex = i;
        }
      }
      //cout << "aire : " << maxArea << endl;
      //cout << "index : " << maxAreaIndex << endl;

      // on dessine le contour de la feuille sur l'image originale frame
      drawContours(frame, contours, maxAreaIndex, Scalar(0, 0, 255), 2);





      // remplis la texture avec ces pixels
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    }

    // on dessine le carré dans lequel on va mettre la texture
    glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex2f(-1, -1);   // coin bas gauche
      glTexCoord2f(1, 0); glVertex2f( 1, -1);   // coin bas droit
      glTexCoord2f(1, 1); glVertex2f( 1, 1);   // coin haut droit
      glTexCoord2f(0, 1); glVertex2f(-1, 1);   // coin haut gauche
    glEnd();

    // back buffer (dessin) devient le front buffer (visible)
    glfwSwapBuffers(window);
    // récupère les évènement (clics, souris, etc)
    glfwPollEvents();
}
  std::cout << glfwGetTime() << endl;
  // nettoyage  
  glDeleteTextures(1, &textureID);
  video.release();

  // ferme proprement toutes les ressources de GLFW (pendant de glfwInit())
  glfwTerminate();
  return 0;
}