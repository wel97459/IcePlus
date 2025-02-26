import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;
import java.io.InputStream;
import java.util.Random;
import javax.sound.sampled.*;
import java.io.File;
import java.io.IOException;
import java.awt.image.BufferedImage;

public class IcePlus extends JFrame implements Runnable, MouseListener, KeyListener {
   private final String rawFolder = "raw/";
   private final String soundsFolder = "sounds/";
   private int gameMode = 0;
   private int counter;
   private int mazeCount;
   private int levGround;
   private int lastKey;
   private Image background;
   private Image backbuffer;
   private Graphics gg;
   private Graphics bgg;
   Thread runner;
   private long nextTime;
   private Random rnd = new Random();
   private Font f = new Font("SansSerif", 1, 14);
   private FontMetrics fm;
   private MediaTracker mt;
   private IndexColorModel icm;
   private IndexColorModel opaque;
   final int[] spriteH = new int[]{ 29, 32, 31, 29, 31, 31, 29, 31, 32, 32, 30, 32, 32, 30, 24, 27, 30, 30, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 30, 29, 27, 25, 23, 22, 25, 27, 29, 27, 15, 15, 15, 15, 16, 12
   };
   final int[] spriteEffH = new int[]{ 29, 32, 31, 29, 31, 31, 29, 31, 32, 32, 30, 32, 32, 30, 24, 27, 30, 30, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 30, 29, 27, 25, 23, 22, 25, 27, 29, 27, 22, 22, 22, 22, 16, 12
   };
   private final byte[] shadowX = new byte[]{ 5, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 8, 8, 8, 8, 0, 0, 0, 8, 8, 8, 8, 6, 6, 6, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   private int[] spriteStart;
   private int[] map = new int[168];
   private byte[] ground = new byte[672];
   private final byte[] shadBytes = new byte[]{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
   private byte[] shadByte = new byte[1536];
   private byte[] logo;
   private int[] green = new int[4480];
   private int score;
   private int[] highscores = new int[8];
   private int nextMode;
   private int startLevel;
   private int maxLevel;
   private int soundOn = 1;
   private int introCount;
   private int coins;
   private int lives;
   private int level;
   private int highRow;
   private int highCol;
   private int offsX;
   private int offsY;
   private int clipX;
   private int clipY;
   private int clipW;
   private int clipH;
   private iceObject[] objs = new iceObject[10];
   private int[] sortY = new int[10];
   private int[] sortIX = new int[10];
   private int sortN;
   private int[] enemies = new int[4];
   private byte[] levFl1 = new byte[]{1, 2, 2, 3, 3, 1, 1, 2, 2, 4, 4, 1, 1, 2, 2, 0, 0, 1, 1, 0};
   private byte[] levFl2 = new byte[]{0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 2, 2, 2, 2, 3, 3, 3, 3, 4};
   private byte[] levIce = new byte[]{12, 12, 13, 13, 14, 14, 16, 16, 12, 12, 12, 17, 17, 12, 12, 18, 18, 12, 12, 12};
   private byte[] levRock = new byte[]{7, 7, 7, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6};
   private byte[] cyclic = new byte[]{0, 1, 2, 1};
   private byte[] cyclic2 = new byte[]{0, 1, 2, 3, 2, 1};
   private byte[] pStepX = new byte[]{3, 4, 3, 4, 3, 4, 3};
   private byte[] pStepY = new byte[]{2, 3, 2, 2, 3, 2, 2};
   private byte[] iStepX = new byte[]{6, 6, 6, 6};
   private byte[] iStepY = new byte[]{4, 4, 4, 4};
   private byte[] fStepX = new byte[]{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
   private byte[] fStepY = new byte[]{1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2};
   private byte[] rStepX = new byte[]{3, 3, 3, 3, 3, 3, 3, 3};
   private byte[] rStepY = new byte[]{2, 2, 2, 2, 2, 2, 2, 2};
   private byte[] offsets = new byte[]{0, -12, 12, -1, 1};
   private byte[] blues = new byte[256];
   private byte[] greens = new byte[256];
   private byte[] reds = new byte[256];
   private byte[] trans = new byte[256];
   private byte[] spriteByte;
   private byte[] backByte = new byte[6720];
   private final String[] levNames = new String[]{
      "bigdirt.raw",
      "biglego.raw",
      "bigbeach.raw",
      "biggrass.raw",
      "bigweird.raw",
      "bigdice.raw",
      "bigwood.raw",
      "bigapples.raw",
      "bigboxes.raw",
      "bigbooks.raw"
   };
   private final short[] levLengths = new short[]{3072, 2496, 3072, 2880, 2880, 2880, 3072, 3264, 3072, 3264};
   private final String[] INTRO = new String[]{"Written and designed", "By Karl Hörnell", "(c) OnGame, 2002"};
   private final String[] HELP = new String[]{
      "You control Pixel",
      "Pete, the penguin.",
      "Move ice blocks by",
      "walking against them",
      "or crush them",
      "against other blocks.",
      "Watch out for",
      "the flames.",
      "You can snuff",
      "them with ice.",
      "Collect all five",
      "golden coins ...",
      "... to get to",
      "the next level."
   };
   private char[][] HIGHSCORES = new char[8][14];
   private String[] MENUS = new String[]{"Play", "Settings", "Exit", "Start on level", "Sound", "Back", "OFF", "ON"};
   private Color[] menuColor = new Color[]{Color.white, Color.darkGray};
   char[] LEVELTEXT = new char[]{'L', 'e', 'v', 'e', 'l', ' ', '0', '1'};
   private String[] VARIOUS = new String[]{"Highscores", "GAME OVER", "DEMO"};
   char[] scString = new char[5];
   private int[] menuX = new int[6];
   private int[] menuY = new int[6];
   private int scoreX;
   private int selected;
   private final int[] motionKeys = new int[]{0, 38, 40, 37, 39};
   private int certified = 1;
   private Clip[] sounds;
   private Image[] sprites = new Image[this.spriteH.length];
   private Image logoIm;
   private Image greenIm;
   private Image[] props = new Image[17];
   private Image[] shadows = new Image[4];
   private int[] propO = new int[]{0, 768, 1536, 2304, 3072, 3840, 4608, 5760, 5772, 5952, 5964, 6144, 6156, 6336, 6348, 6528, 6540};
   private int[] propW = new int[]{24, 24, 24, 24, 24, 24, 72, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};
   private int[] propH = new int[]{32, 32, 32, 32, 32, 32, 16, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
   private int textDy;

   public void init() {
      this.mt = new MediaTracker(this);

      for (int var1 = 1; var1 < 256; var1++) {
         this.reds[var1] = (byte)(36 * (var1 >> 5));
         this.greens[var1] = (byte)(36 * (var1 >> 2 & 7));
         this.blues[var1] = (byte)(85 * (var1 & 3));
         this.trans[var1] = -1;
      }

      this.icm = new IndexColorModel(8, 256, this.reds, this.greens, this.blues, this.trans);
      this.opaque = new IndexColorModel(8, 256, this.reds, this.greens, this.blues);
      this.spriteByte = new byte[32208];

      try {
         InputStream var5 = this.getClass().getResourceAsStream(rawFolder + "sprites1.raw");

         for (int var8 = 0; var8 < 13008; var8++) {
            this.spriteByte[var8] = (byte)var5.read();
         }

         var5.close();
         var5 = this.getClass().getResourceAsStream(rawFolder +  "sprites2.raw");

         for (int var9 = 0; var9 < 10752; var9++) {
            this.spriteByte[13008 + var9] = (byte)var5.read();
         }

         var5.close();
         var5 = this.getClass().getResourceAsStream(rawFolder + "sprites3.raw");

         for (int var10 = 0; var10 < 8448; var10++) {
            this.spriteByte[23760 + var10] = (byte)var5.read();
         }

         var5.close();
      } catch (Exception var7) {
         var7.printStackTrace();
      }

      this.prepareBackData();

      for (int var11 = 0; var11 < 17; var11++) {
         this.props[var11] = this.createImage(
            new MemoryImageSource(this.propW[var11], this.propH[var11], this.opaque, this.backByte, this.propO[var11], Math.max(24, this.propW[var11]))
         );
         this.mt.addImage(this.props[var11], 1);
      }

      int var2 = 0;
      this.spriteStart = new int[this.spriteH.length];

      for (int var12 = 0; var12 < this.spriteH.length; var12++) {
         this.spriteStart[var12] = var2;
         var2 += 24 * this.spriteH[var12];
         this.sprites[var12] = this.createImage(new MemoryImageSource(24, this.spriteH[var12], this.icm, this.spriteByte, this.spriteStart[var12], 24));
         this.mt.addImage(this.sprites[var12], 0);
      }

      for (int var13 = 0; var13 < 4; var13++) {
         this.shadows[var13] = this.createImage(new MemoryImageSource(24, 16, this.icm, this.shadByte, 384 * var13, 24));
         this.mt.addImage(this.shadows[var13], 1);
      }

      this.logo = this.loadPixels(rawFolder + "logo.raw", 10208);
      this.logoIm = this.createImage(new MemoryImageSource(176, 58, this.opaque, this.logo, 0, 176));
      this.mt.addImage(this.logoIm, 0);
      boolean var4 = false;

      for (int var14 = 0; var14 < 4480; var14++) {
         this.green[var14] = -16777216 + (var14 / 22 << 8);
      }

      this.greenIm = this.createImage(new MemoryImageSource(32, 140, this.green, 0, 32));
      this.mt.addImage(this.greenIm, 0);

      for (int var15 = 0; var15 < 10; var15++) {
         this.objs[var15] = new iceObject();
      }

      this.fm = this.getFontMetrics(this.f);
      this.textDy = this.fm.getAscent() - 1;
      this.background = new BufferedImage(176, 208, BufferedImage.TYPE_INT_RGB);
      this.gg = this.background.getGraphics();
      this.backbuffer = new BufferedImage(276, 248, BufferedImage.TYPE_INT_RGB);
      this.bgg = this.backbuffer.getGraphics();
      this.readPermanent();
      this.initialize();

      try {
         this.mt.waitForAll();
      } catch (InterruptedException var6) {
      }

      // Initialize sounds
      try {
         sounds = new Clip[6];
         for (int i = 0; i < 6; i++) {
               // Load sound files from the local filesystem
               File soundFile = new File(soundsFolder + "sound" + i + ".wav"); // Use .wav files
               if (soundFile.exists()) {
                  sounds[i] = AudioSystem.getClip();
                  sounds[i].open(AudioSystem.getAudioInputStream(soundFile));
               } else {
                  System.err.println("Sound file not found: " + soundFile.getAbsolutePath());
               }
         }
      } catch (UnsupportedAudioFileException | IOException | LineUnavailableException e) {
         System.err.println("Error loading sounds: " + e.getMessage());
      }

      this.addMouseListener(this);
      this.addKeyListener(this);
      this.runner = new Thread(this);
      this.runner.start();
   }

   // Helper method to play sounds
   private void playSound(int index) {
      try {
         if (sounds != null && index >= 0 && index < sounds.length) {
               if (sounds[index] != null && !sounds[index].isRunning()) {
                  sounds[index].setFramePosition(0);
                  sounds[index].start();
               }
         }
      } catch (Exception e) {
         System.err.println("Error playing sound: " + e.getMessage());
      }
   }

   private void readPermanent() {
      if (this.HIGHSCORES[0][0] == 0) {
         for (int var1 = 0; var1 < 8; var1++) {
            int var3 = 0;
            this.HIGHSCORES[var1][var3++] = (char)(49 + var1);
            this.HIGHSCORES[var1][var3++] = '.';
            this.HIGHSCORES[var1][var3++] = ' ';
            this.HIGHSCORES[var1][var3++] = ' ';

            for (int var2 = 0; var2 < 5; var2++) {
               this.HIGHSCORES[var1][var3++] = '0';
            }

            this.HIGHSCORES[var1][var3++] = ' ';
            this.HIGHSCORES[var1][var3++] = ' ';

            for (int var4 = 0; var4 < 3; var4++) {
               this.HIGHSCORES[var1][var3++] = '.';
            }
         }

         this.maxLevel = 4;
      }
   }

   private void storePermanent() {
   }

   private void prepareBackData() {
      for (int var1 = 0; var1 < 768; var1++) {
         this.backByte[var1] = this.spriteByte[var1 + 13008];
         this.backByte[var1 + 768] = this.spriteByte[var1 + 13008];
         this.backByte[var1 + 1536] = this.spriteByte[var1 + 18384];
         this.backByte[var1 + 2304] = this.spriteByte[var1 + 18384];
      }

      byte[] var5 = this.loadPixels(rawFolder + "bigiceshadow.raw", 256);
      int var3 = 1152;
      int var4 = 0;

      for (int var6 = 0; var6 < 16; var6++) {
         for (int var2 = 0; var2 < 8; var2++) {
            this.backByte[var3++] = var5[var4++];
         }

         var3 += 16;
      }

      var3 += 1152;

      for (int var7 = 0; var7 < 16; var7++) {
         for (int var8 = 0; var8 < 8; var8++) {
            this.backByte[var3++] = var5[var4++];
         }

         var3 += 16;
      }
   }

   void setUpIntroScreen() {
      this.gg.setColor(Color.black);
      this.gg.fillRect(0, 0, 176, 10);

      for (int var1 = 0; var1 < 6; var1++) {
         this.gg.drawImage(this.greenIm, var1 * 32, 68, this);
      }

      for (int var2 = 0; var2 < 3; var2++) {
         this.objs[var2].x = -36;
         this.objs[var2].y = 178;
         this.objs[var2].look = 0;
      }

      this.introCount = -1;
      this.prepareMenu(0);
      this.buildShadows((byte)12);
      this.advanceIntro();
      this.scoreX = 0;

      for (int var3 = 0; var3 < 8; var3++) {
         this.scoreX = this.scoreX + this.fm.charsWidth(this.HIGHSCORES[var3], 0, 14);
      }

      this.scoreX = 88 - this.scoreX / 16;
      this.gameMode = 2;
   }

   void prepareIntro(int var1) {
      this.gg.setColor(Color.black);
      this.gg.fillRect(0, 0, 176, 10);
      this.gg.drawImage(this.logoIm, 0, 10, this);
      if (this.certified == 0) {
         this.vPrint(0, 0, this.VARIOUS[2], Color.white);
      }

      for (int var2 = 0; var2 < 6; var2++) {
         this.gg.drawImage(this.greenIm, var2 * 32, 68, this);
      }

      this.gameMode = 0;
      this.adjustStrings();
      this.nextTime = System.currentTimeMillis() + var1;
      this.nextMode = 1;
   }

   void adjustStrings() {
      for (int var1 = 0; var1 < 3; var1++) {
         this.menuY[var1] = 205 - this.fm.getHeight();
      }

      this.menuX[0] = 2;
      this.menuX[1] = 88 - this.fm.stringWidth(this.MENUS[1]) / 2;
      this.menuX[2] = 174 - this.fm.stringWidth(this.MENUS[2]);

      for (int var2 = 3; var2 < 6; var2++) {
         this.menuY[var2] = 50 + 20 * (var2 - 3);
      }

      this.menuX[3] = (158 - this.fm.stringWidth(this.MENUS[3])) / 2;
      this.menuX[4] = (154 - this.fm.stringWidth(this.MENUS[4])) / 2;
      this.menuX[5] = 88 - this.fm.stringWidth(this.MENUS[5]) / 2;
   }

   void initialize() {
      this.gg.setFont(this.f);
      this.bgg.setFont(this.f);
      this.rnd.setSeed(System.currentTimeMillis());
      this.prepareIntro(2000);

      for (int var1 = 0; var1 < 3; var1++) {
         this.vPrint(88 - this.fm.stringWidth(this.INTRO[var1]) / 2, 90 + 16 * var1, this.INTRO[var1], Color.white);
      }
   }

   void vPrint(int var1, int var2, String var3, Color var4) {
      this.gg.setColor(Color.black);
      this.gg.drawString(var3, var1 + 1, var2 + 1 + this.textDy);
      this.gg.setColor(var4);
      this.gg.drawString(var3, var1, var2 + this.textDy);
   }

   void vPrint(int var1, int var2, char[] var3, int var4, int var5) {
      this.gg.setColor(Color.black);
      this.gg.drawChars(var3, var4, var5, var1 + 1, var2 + 1 + this.textDy);
      this.gg.setColor(Color.white);
      this.gg.drawChars(var3, var4, var5, var1, var2 + this.textDy);
   }

   void prepareMenu(int var1) {
      this.selected = var1;

      for (int var2 = 0; var2 < 3; var2++) {
         this.vPrint(this.menuX[var1 + var2], this.menuY[var1 + var2], this.MENUS[var1 + var2], Color.white);
      }
   }

   void advanceIntro() {
      this.introCount++;
      if (this.introCount > 8) {
         this.introCount = 0;
      }

      this.counter = 0;
      if (this.introCount < 7) {
         this.gg.setClip(0, 68, 176, 50);

         for (int var1 = 0; var1 < 6; var1++) {
            this.gg.drawImage(this.greenIm, var1 * 32, 68, this);
         }

         this.gg.setClip(0, 0, 176, 208);
         this.vPrint(20, 82, this.HELP[this.introCount << 1], Color.white);
         this.vPrint(20, 98, this.HELP[(this.introCount << 1) + 1], Color.white);
      } else {
         this.gg.setClip(0, 68, 176, 120);

         for (int var4 = 0; var4 < 6; var4++) {
            this.gg.drawImage(this.greenIm, var4 * 32, 68, this);
         }

         this.gg.setClip(0, 0, 176, 208);
         this.vPrint(88 - this.fm.stringWidth(this.VARIOUS[0]) / 2, 80, this.VARIOUS[0], Color.white);
         int var5 = this.introCount - 7 << 2;
         byte var3 = 102;

         for (int var2 = var5; var2 < var5 + 4; var2++) {
            this.vPrint(this.scoreX, var3, this.HIGHSCORES[var2], 0, 14);
            var3 += 16;
         }
      }
   }

   void animateIntro() {
      if (this.introCount < 7) {
         this.gg.setClip(0, 118, 176, 70);

         for (int var1 = 0; var1 < 6; var1++) {
            this.gg.drawImage(this.greenIm, var1 * 32, 68, this);
         }

         this.gg.setClip(0, 0, 176, 208);
      }

      switch (this.introCount) {
         case 0:
            if (this.objs[0].x < 144) {
               this.objs[0].look = 11 + this.cyclic[(this.counter & 7) >> 1];
               this.objs[0].x += 4;
            } else if (this.objs[1].x < 96) {
               this.objs[0].look = 6;
               this.objs[1].look = 18;
               this.objs[1].x += 4;
            } else if (this.objs[2].x < 24) {
               this.objs[2].look = 25;
               this.objs[2].x += 4;
            } else {
               this.advanceIntro();
            }
            break;
         case 1:
            if (this.objs[0].x > 120) {
               this.objs[0].x -= 4;
               this.objs[0].look = 8 + this.cyclic[(this.counter & 7) >> 1];
            } else if (this.objs[1].x > 48) {
               this.objs[1].x -= 6;
            } else if (this.counter > 27) {
               this.advanceIntro();
            }
            break;
         case 2:
            if (this.objs[0].x > 72) {
               this.objs[0].x -= 4;
               this.objs[0].look = 8 + this.cyclic[(this.counter & 7) >> 1];
            } else if (this.objs[1].look < 24) {
               this.objs[1].look = this.objs[1].look + (this.counter & 1);
            } else if (this.objs[0].x > 48) {
               this.objs[1].x = -36;
               this.objs[0].x -= 4;
               this.objs[0].look = 8 + this.cyclic[(this.counter & 7) >> 1];
            } else {
               this.objs[0].look = 6;
               this.objs[1].x = 204;
               this.advanceIntro();
            }
            break;
         case 3:
            this.objs[1].look = 14 + this.cyclic2[this.counter % 6];
            if (this.counter < 25) {
               this.objs[1].x -= 4;
            } else if (this.counter >= 40) {
               if (this.counter < 65) {
                  this.objs[1].x += 4;
               } else {
                  this.objs[1].x = -24;
                  this.advanceIntro();
               }
            }
            break;
         case 4:
            if (this.objs[1].x < this.objs[2].x) {
               this.objs[1].look = 14 + this.cyclic2[this.counter % 6];
               this.objs[1].x += 4;
            } else {
               this.objs[1].look = 42 + (this.counter & 1);
               this.objs[1].x++;
            }

            this.objs[2].x -= 6;
            if (this.counter < 32) {
               this.objs[0].look = 10;
            } else {
               this.objs[0].look = 6;
            }

            if (this.counter > 39) {
               this.objs[1].look = 18;
               this.objs[1].x = 204;
               this.objs[2].x = 204;
               this.advanceIntro();
            }
            break;
         case 5:
            if (this.objs[2].x > 120) {
               this.objs[2].x -= 4;
            } else if (this.objs[1].x > 144) {
               this.objs[1].x -= 4;
            } else if (this.objs[0].x < 96) {
               this.objs[0].x += 4;
               this.objs[0].look = 11 + this.cyclic[(this.counter & 7) >> 1];
            } else {
               this.advanceIntro();
            }
            break;
         case 6:
            if (this.objs[2].look < 31) {
               this.objs[2].look = this.objs[2].look + (this.counter & 1);
            } else if (this.counter < 45) {
               this.objs[2].x = -36;
               this.objs[0].look = (this.counter & 4) >> 2;
            } else {
               this.objs[0].x = -36;
               this.objs[1].x = -36;
               this.objs[0].look = 0;
               this.objs[1].look = 0;
               this.advanceIntro();
            }
            break;
         default:
            if (this.counter > 50) {
               this.advanceIntro();
            }
      }
   }

   void drawSpriteSimple(int var1) {
      this.gg.drawImage(this.sprites[this.objs[var1].look], this.objs[var1].x, this.objs[var1].y - this.spriteEffH[this.objs[var1].look], this);
   }

   void drawShadowSimple(int var1) {
      byte var2 = 0;
      if (this.shadowX[this.objs[var1].look] == 8) {
         var2 = 2;
      }

      if (this.shadowX[this.objs[var1].look] > 0) {
         this.gg.drawImage(this.shadows[var2], this.objs[var1].x + this.shadowX[this.objs[var1].look], this.objs[var1].y - 16, this);
      }
   }

   void addObject(int var1, int var2, int var3, int var4) {
      int var5 = 0;

      while (var5 < 9 && this.objs[var5].type > 0) {
         var5++;
      }

      this.objs[var5].type = var1;
      this.objs[var5].pos = var2;
      this.objs[var5].look = var3;
      this.objs[var5].dir = var4;
      this.objs[var5].x = var2 % 12 * 24;
      this.objs[var5].y = var2 / 12 << 4;
      this.objs[var5].step = 0;
   }

   void buildMap(int[] var1) {
      short[] var7 = new short[180];
      int var6 = 0;

      while (var6 < 5) {
         for (int var2 = 0; var2 < 14; var2++) {
            for (int var3 = 0; var3 < 12; var3++) {
               int var4 = var2 * 12 + var3;
               if (var2 > 0 && var2 < 13 && var3 > 0 && var3 < 11) {
                  this.map[var4] = 0;
               } else {
                  this.map[var4] = 3;
               }
            }
         }

         this.map[13] = 4;
         this.map[167] = 0;

         for (int var11 = 1; var11 < 4; var11++) {
            for (int var8 = 0; var8 < var1[var11]; var8++) {
               int var13 = 0;

               while (this.map[var13] != 0) {
                  var13 = 12 + (this.rnd.nextInt() & 4095) % 144;
                  if (this.map[var13] != 0) {
                     var13 = 0;
                  } else {
                     this.map[var13] = var11;
                     var13 = 167;
                  }
               }
            }
         }

         this.map[13] = 4;
         var7[0] = 13;
         int var9 = 0;
         int var12 = 1;
         var6 = 0;

         while (var9 < var12) {
            short var15 = var7[var9++];

            for (int var5 = 1; var5 < 5; var5++) {
               if (this.map[var15 + this.offsets[var5]] < 3) {
                  var7[var12++] = (short)(var15 + this.offsets[var5]);
                  if (this.map[var15 + this.offsets[var5]] == 2) {
                     var6++;
                  }

                  this.map[var15 + this.offsets[var5]] = this.map[var15 + this.offsets[var5]] | 4;
               }
            }
         }
      }

      this.map[167] = 3;

      for (int var10 = 0; var10 < 168; var10++) {
         this.map[var10] = this.map[var10] & 3;
      }
   }

   void buildShadows(byte var1) {
      int var5 = 0;
      int var7 = 768;
      if (var1 == 0) {
         for (int var4 = 0; var4 < 2; var4++) {
            for (int var2 = 0; var2 < 16; var2++) {
               int var6 = var5;

               for (int var3 = 0; var3 < 24; var3++) {
                  this.shadByte[var7++] = (byte)var6;
                  var6 = 32 - var6;
               }

               var5 = 32 - var5;
            }

            var5 = 32 - var5;
         }
      } else {
         for (int var10 = 0; var10 < 768; var10++) {
            this.shadByte[768 + var10] = var1;
         }
      }

      for (int var11 = 0; var11 < 384; var11++) {
         if (this.shadBytes[var11] != 0) {
            this.shadByte[var11] = this.shadByte[var11 + 768];
            this.shadByte[var11 + 384] = this.shadByte[var11 + 1152];
         }
      }

      for (int var12 = 0; var12 < 4; var12++) {
         this.shadows[var12].flush();
      }

      try {
         this.mt.waitForID(1);
      } catch (InterruptedException var9) {
      }
   }

   public void run() {
      int var7 = 0;

      try {
         while (true) {
            long var8 = System.currentTimeMillis();
            this.counter++;
            switch (this.gameMode) {
               case 0:
                  if (System.currentTimeMillis() > this.nextTime) {
                     this.gameMode = this.nextMode;
                  }
                  break;
               case 1:
                  this.setUpIntroScreen();
                  break;
               case 2:
                  this.animateIntro();

                  for (int var22 = 0; var22 < 3; var22++) {
                     this.drawShadowSimple(var22);
                  }

                  for (int var23 = 0; var23 < 3; var23++) {
                     this.drawSpriteSimple(var23);
                  }

                  this.lastKey &= 255;
                  if (this.lastKey != var7) {
                     var7 = this.lastKey;
                     if (this.lastKey == 37) {
                        if (this.selected > 0) {
                           this.vPrint(this.menuX[this.selected], this.menuY[this.selected], this.MENUS[this.selected], Color.white);
                           this.selected--;
                        }
                     } else if (this.lastKey == 39 && this.selected < 2) {
                        this.vPrint(this.menuX[this.selected], this.menuY[this.selected], this.MENUS[this.selected], Color.white);
                        this.selected++;
                     }
                  }

                  this.vPrint(this.menuX[this.selected], this.menuY[this.selected], this.MENUS[this.selected], this.menuColor[(this.counter & 4) >> 2]);
                  if (this.lastKey == 32 || this.lastKey == 10) {
                     if (this.selected == 0) {
                        this.startSession();
                     } else if (this.selected == 1) {
                        this.prepareSettings();
                     }
                  }
                  break;
               case 3:
                  this.gg.setColor(Color.black);
                  this.gg.fillRect(this.clipX, this.clipY, this.clipW, this.clipH);
                  this.clipX -= 8;
                  this.clipY -= 10;
                  this.clipW += 16;
                  this.clipH += 20;
                  if (this.clipX >= 0) {
                     break;
                  }

                  this.clipX = 80;
                  this.clipW = 16;
                  this.clipY = 100;
                  this.clipH = 8;

                  for (int var19 = 1; var19 < 10; var19++) {
                     this.objs[var19].type = 0;
                  }

                  this.gameMode = 5;
                  this.gg.setColor(Color.white);
                  this.LEVELTEXT[6] = (char)(48 + (this.level + 1) / 10 % 10);
                  this.LEVELTEXT[7] = (char)(48 + (this.level + 1) % 10);
                  this.gg.drawChars(this.LEVELTEXT, 0, 8, 88 - this.fm.charsWidth(this.LEVELTEXT, 0, 8) / 2, 50 + this.textDy);
                  this.counter = 0;
                  this.offsX = 0;
                  this.offsY = 0;
                  this.prepareEnemies();
                  byte var29 = 31;
                  int var20 = 3;

                  for (this.objs[0].y = 105; this.enemies[var20] == 0; var20--) {
                     var29 += 15;
                  }

                  for (int var21 = 0; var21 < 4; var21++) {
                     this.objs[0].x = var29;
                     var29 += 30;
                     if (this.enemies[var21] > 0) {
                        if (this.enemies[var21] == 6) {
                           this.objs[0].look = 15;
                        } else {
                           this.objs[0].look = 38;
                        }

                        this.drawSpriteSimple(0);
                     }
                  }
                  break;
               case 4:
                  this.gg.setClip(this.clipX, this.clipY, this.clipW, this.clipH);
                  this.gg.drawImage(this.backbuffer, -12, 0, this);
                  this.clipX -= 8;
                  this.clipY -= 10;
                  this.clipW += 16;
                  this.clipH += 20;
                  if (this.clipX < 0) {
                     this.gameMode = 6;
                  }
                  break;
               case 5:
                  if (this.counter > 12) {
                     this.prepareLevel();
                     this.gameMode = 4;
                  }
                  break;
               case 6:
                  int var14 = this.counter >> 4 & 3;
                  if (this.enemies[var14] > 0) {
                     byte var2;
                     byte var3;
                     if (this.objs[0].x < 144) {
                        var2 = 23;
                        var3 = 3;
                     } else {
                        var2 = 12;
                        var3 = 4;
                     }

                     var2 += 12 * (this.rnd.nextInt() & 7);

                     while (var2 < 146 && this.map[var2 + this.offsets[var3]] > 0) {
                        var2 += 12;
                     }

                     if (var2 < 146) {
                        this.addObject(this.enemies[var14], var2, 0, var3);
                        this.enemies[var14] = 0;
                     }
                  }

                  this.lastKey &= 255;

                  for (int var15 = 0; var15 < 10; var15++) {
                     switch (this.objs[var15].type) {
                        case 1:
                           if (this.objs[var15].x % 24 == 0 && (this.objs[var15].y & 15) == 0) {
                              this.objs[var15].step = 0;
                              this.objs[var15].dir = 0;

                              for (int var28 = 1; var28 < 5; var28++) {
                                 if (this.lastKey == this.motionKeys[var28]) {
                                    int var31 = this.objs[var15].pos + this.offsets[var28];
                                    if (this.map[var31] == 0) {
                                       this.objs[var15].dir = var28;
                                       this.objs[var15].pos = this.objs[var15].pos + this.offsets[var28];
                                    } else if (this.map[var31] == 1 || this.map[var31] == 2) {
                                       if ((this.map[var31 + this.offsets[var28]] & 3) == 0) {
                                          this.addObject(this.map[var31] + 1, var31, 11 + 7 * this.map[var31], var28);
                                          this.map[var31] = 0;
                                          this.removeBlock(var31);
                                       } else {
                                          if (this.soundOn != 0) {
                                             this.playSound(1);
                                          }

                                          this.addObject(this.map[var31] + 3, var31, 11 + 7 * this.map[var31], 0);
                                          this.map[var31] = 4;
                                          this.removeBlock(var31);
                                       }

                                       this.objs[var15].look = var28 * 3 - 1 + this.cyclic[(this.counter & 6) >> 1];
                                    }
                                 }
                              }
                           }

                           if (this.objs[var15].dir > 0) {
                              switch (this.objs[var15].dir) {
                                 case 1:
                                    this.objs[var15].y = this.objs[var15].y - this.pStepY[this.objs[var15].step];
                                    break;
                                 case 2:
                                    this.objs[var15].y = this.objs[var15].y + this.pStepY[this.objs[var15].step];
                                    break;
                                 case 3:
                                    this.objs[var15].x = this.objs[var15].x - this.pStepX[this.objs[var15].step];
                                    break;
                                 case 4:
                                    this.objs[var15].x = this.objs[var15].x + this.pStepX[this.objs[var15].step];
                              }

                              this.objs[var15].look = this.objs[var15].dir * 3 - 1 + this.cyclic[(this.counter & 6) >> 1];
                              this.objs[var15].step++;
                           }

                           this.offsX = this.objs[var15].x - 76;
                           if (this.offsX < 12) {
                              this.offsX = 12;
                           } else if (this.offsX > 100) {
                              this.offsX = 100;
                           }

                           this.offsY = this.objs[var15].y - 110;
                           if (this.offsY < 0) {
                              this.offsY = 0;
                           }

                           if (this.offsY > 40) {
                              this.offsY = 40;
                           }
                           break;
                        case 2:
                        case 3:
                           switch (this.objs[var15].dir) {
                              case 1:
                                 this.objs[var15].y = this.objs[var15].y - this.iStepY[this.objs[var15].step];
                                 break;
                              case 2:
                                 this.objs[var15].y = this.objs[var15].y + this.iStepY[this.objs[var15].step];
                                 break;
                              case 3:
                                 this.objs[var15].x = this.objs[var15].x - this.iStepX[this.objs[var15].step];
                                 break;
                              case 4:
                                 this.objs[var15].x = this.objs[var15].x + this.iStepX[this.objs[var15].step];
                           }

                           this.objs[var15].step++;
                           if (this.objs[var15].x % 24 == 0 && (this.objs[var15].y & 15) == 0) {
                              this.objs[var15].step = 0;
                              this.objs[var15].pos = this.objs[var15].pos + this.offsets[this.objs[var15].dir];
                              if (this.map[this.objs[var15].pos + this.offsets[this.objs[var15].dir]] > 0) {
                                 this.addBlock(this.objs[var15].pos, this.objs[var15].type - 1);
                                 this.objs[var15].type = 0;
                              }
                           }

                           for (int var27 = 1; var27 < 10; var27++) {
                              if ((this.objs[var27].type & 14) == 6
                                 && this.objs[var15].x < this.objs[var27].x + 19
                                 && this.objs[var27].x < this.objs[var15].x + 19
                                 && this.objs[var15].y < this.objs[var27].y + 12
                                 && this.objs[var27].y < this.objs[var15].y + 12) {
                                 if (this.soundOn != 0) {
                                    this.playSound(0);
                                 }

                                 if (this.objs[var27].type == 6) {
                                    this.objs[var27].look = 42;
                                    this.updateScore(50);
                                 } else {
                                    this.objs[var27].look = 44;
                                    this.updateScore(100);
                                 }

                                 int var30 = 0;

                                 while (this.enemies[var30] > 0) {
                                    var30++;
                                 }

                                 this.enemies[var30] = this.objs[var27].type;
                                 this.objs[var27].type = 8;
                                 this.objs[var27].dir = 0;
                              }
                           }
                           break;
                        case 4:
                        case 5:
                           this.objs[var15].look++;
                           if (this.objs[var15].look != 25 && this.objs[var15].look != 32) {
                              break;
                           }

                           this.map[this.objs[var15].pos] = 0;
                           if (this.objs[var15].type == 5) {
                              if (this.soundOn != 0) {
                                 this.playSound(2);
                              }

                              this.coins++;
                              this.updateScore(25);
                              if (this.coins == 5) {
                                 this.objs[0].look = 1;
                                 this.gameMode = 7;
                                 this.counter = 0;
                              }
                           } else {
                              this.updateScore(5);
                           }

                           this.objs[var15].type = 0;
                           break;
                        case 6:
                        case 7:
                           if (this.objs[var15].type == 6) {
                              this.objs[var15].look = 14 + this.cyclic2[this.counter % 6];
                              switch (this.objs[var15].dir) {
                                 case 1:
                                    this.objs[var15].y = this.objs[var15].y - this.fStepY[this.objs[var15].step];
                                    break;
                                 case 2:
                                    this.objs[var15].y = this.objs[var15].y + this.fStepY[this.objs[var15].step];
                                    break;
                                 case 3:
                                    this.objs[var15].x = this.objs[var15].x - this.fStepX[this.objs[var15].step];
                                    break;
                                 case 4:
                                    this.objs[var15].x = this.objs[var15].x + this.fStepX[this.objs[var15].step];
                              }
                           } else {
                              this.objs[var15].look = 38 + (this.counter & 3);
                              switch (this.objs[var15].dir) {
                                 case 1:
                                    this.objs[var15].y = this.objs[var15].y - this.rStepY[this.objs[var15].step];
                                    break;
                                 case 2:
                                    this.objs[var15].y = this.objs[var15].y + this.rStepY[this.objs[var15].step];
                                    break;
                                 case 3:
                                    this.objs[var15].x = this.objs[var15].x - this.rStepX[this.objs[var15].step];
                                    break;
                                 case 4:
                                    this.objs[var15].x = this.objs[var15].x + this.rStepX[this.objs[var15].step];
                              }
                           }

                           this.objs[var15].step++;
                           if (this.objs[var15].x % 24 == 0 && (this.objs[var15].y & 15) == 0) {
                              this.objs[var15].pos = this.objs[var15].pos + this.offsets[this.objs[var15].dir];
                              this.objs[var15].step = 0;
                              if (this.objs[var15].dir == 0 || (this.counter & 14) == 0) {
                                 this.objs[var15].dir = 1 + (this.rnd.nextInt() & 3);
                              }

                              if (this.objs[var15].x < this.objs[0].x + 15 && this.objs[var15].x > this.objs[0].x - 15) {
                                 if (this.objs[var15].y < this.objs[0].y) {
                                    this.objs[var15].dir = 2;
                                 } else {
                                    this.objs[var15].dir = 1;
                                 }
                              } else if (this.objs[var15].y < this.objs[0].y + 11 && this.objs[var15].y > this.objs[0].y - 11) {
                                 if (this.objs[0].x > this.objs[var15].x) {
                                    this.objs[var15].dir = 4;
                                 } else {
                                    this.objs[var15].dir = 3;
                                 }
                              }

                              if (this.map[this.objs[var15].pos + this.offsets[this.objs[var15].dir]] > 0) {
                                 this.objs[var15].dir = 0;
                              }
                           }

                           if (this.objs[var15].x >= this.objs[0].x + 14
                              || this.objs[var15].x <= this.objs[0].x - 14
                              || this.objs[var15].y >= this.objs[0].y + 9
                              || this.objs[var15].y <= this.objs[0].y - 9) {
                              break;
                           }

                           if (this.soundOn != 0) {
                              this.playSound(3);
                           }

                           this.objs[0].type = 9;
                           this.objs[0].look = 32;

                           for (int var25 = 1; var25 < 10; var25++) {
                              if ((this.objs[var25].type & 14) == 6) {
                                 this.objs[var25].type = 0;
                              }
                           }

                           for (int var26 = 0; var26 < 4; var26++) {
                              this.enemies[var26] = 0;
                           }

                           this.counter = 0;
                           break;
                        case 8:
                           this.objs[var15].look = (this.objs[var15].look & 62) + (this.counter & 1);
                           this.objs[var15].dir++;
                           if (this.objs[var15].dir > 15) {
                              this.objs[var15].type = 0;
                           }
                           break;
                        case 9:
                           if ((this.counter & 1) != 0 && this.objs[var15].look < 37) {
                              this.objs[var15].look++;
                           }

                           if (this.counter > 42) {
                              this.lives--;
                              if (this.lives >= 0) {
                                 this.objs[var15].look = 6;
                                 this.objs[var15].type = 1;
                                 this.prepareEnemies();
                              } else {
                                 if (this.soundOn != 0) {
                                    this.playSound(5);
                                 }

                                 this.gameMode = 8;
                              }
                           }
                     }
                  }

                  this.gg.drawImage(this.backbuffer, -this.offsX, -this.offsY, this);
                  this.sortSprites();

                  for (int var16 = 0; var16 < this.sortN; var16++) {
                     this.drawShadow(this.objs[this.sortIX[var16]]);
                  }

                  for (int var17 = 0; var17 < this.sortN; var17++) {
                     this.drawSprite(this.objs[this.sortIX[var17]]);
                  }

                  int var32 = this.lives;

                  for (short var18 = 158; var32 > 0; var18 -= 19) {
                     this.gg.drawImage(this.sprites[46], var18, 0, this);
                     var32--;
                  }

                  this.gg.setColor(Color.black);
                  this.gg.drawChars(this.scString, 0, 5, 2, 1 + this.textDy);
                  this.gg.setColor(Color.white);
                  this.gg.drawChars(this.scString, 0, 5, 1, this.textDy);
                  break;
               case 7:
                  this.objs[0].look = (this.counter & 4) >> 2;
                  this.gg.drawImage(this.backbuffer, -this.offsX, -this.offsY, this);
                  this.drawShadow(this.objs[0]);
                  this.drawSprite(this.objs[0]);
                  int var5 = this.lives;

                  for (short var1 = 158; var5 > 0; var1 -= 19) {
                     this.gg.drawImage(this.sprites[46], var1, 0, this);
                     var5--;
                  }

                  this.gg.setColor(Color.black);
                  this.gg.drawChars(this.scString, 0, 5, 2, 1 + this.textDy);
                  this.gg.setColor(Color.white);
                  this.gg.drawChars(this.scString, 0, 5, 1, this.textDy);
                  if (this.counter == 10 && this.soundOn != 0) {
                     this.playSound(4);
                  }

                  if (this.counter > 50) {
                     if (this.certified != 0) {
                        this.level++;
                        this.clipX = 80;
                        this.clipW = 16;
                        this.clipY = 100;
                        this.clipH = 8;
                        this.gameMode = 3;
                     } else {
                        this.buildShadows((byte)12);
                        this.prepareIntro(200);
                     }
                  }
                  break;
               case 8:
                  this.vPrint(88 - this.fm.stringWidth(this.VARIOUS[1]) / 2, 100, this.VARIOUS[1], Color.white);
                  if (this.counter <= 100) {
                     break;
                  }

                  if (this.level > this.maxLevel) {
                     this.maxLevel = this.level;
                  }

                  if (this.score > this.highscores[7] && this.certified != 0) {
                     this.prepareHighscores();
                     break;
                  }

                  this.buildShadows((byte)12);
                  this.prepareIntro(200);
                  break;
               case 9:
                  if (this.lastKey != var7) {
                     var7 = this.lastKey;
                     if ((this.lastKey & 0xFF) == 37 && this.highCol > 11) {
                        this.highCol--;
                     } else if ((this.lastKey & 0xFF) == 39 && this.highCol < 13) {
                        this.highCol++;
                     } else if ((this.lastKey & 0xFF) == 38) {
                        if (this.HIGHSCORES[this.highRow][this.highCol] == '.') {
                           this.HIGHSCORES[this.highRow][this.highCol] = 'A';
                        } else if (this.HIGHSCORES[this.highRow][this.highCol] == 'Z') {
                           this.HIGHSCORES[this.highRow][this.highCol] = '.';
                        } else {
                           this.HIGHSCORES[this.highRow][this.highCol]++;
                        }
                     } else if ((this.lastKey & 0xFF) == 40) {
                        if (this.HIGHSCORES[this.highRow][this.highCol] == '.') {
                           this.HIGHSCORES[this.highRow][this.highCol] = 'Z';
                        } else if (this.HIGHSCORES[this.highRow][this.highCol] == 'A') {
                           this.HIGHSCORES[this.highRow][this.highCol] = '.';
                        } else {
                           this.HIGHSCORES[this.highRow][this.highCol]--;
                        }
                     } else if (this.lastKey / 256 > 96 && this.lastKey / 256 < 122) {
                        this.HIGHSCORES[this.highRow][this.highCol] = (char)(this.lastKey / 256 - 32);
                        if (this.highCol < 13) {
                           this.highCol++;
                        }
                     }
                  }

                  this.gg.setColor(new Color(0, 0, 171));
                  this.gg.fillRect(this.scoreX, 36 + 15 * this.highRow, 156 - this.scoreX, 15);
                  this.vPrint(this.scoreX, 36 + this.highRow * 15, this.HIGHSCORES[this.highRow], 0, 14);
                  if ((this.lastKey & 0xFF) == 32 || (this.lastKey & 0xFF) == 10) {
                     this.buildShadows((byte)12);
                     this.storePermanent();
                     this.prepareIntro(200);
                  }
                  break;
               case 10:
                  if (this.lastKey != var7) {
                     var7 = this.lastKey;
                     if ((this.lastKey & 0xFF) == 38) {
                        if (this.selected > 3) {
                           this.vPrint(this.menuX[this.selected], this.menuY[this.selected], this.MENUS[this.selected], Color.white);
                           this.selected--;
                        }
                     } else if ((this.lastKey & 0xFF) == 40) {
                        if (this.selected < 5) {
                           this.vPrint(this.menuX[this.selected], this.menuY[this.selected], this.MENUS[this.selected], Color.white);
                           this.selected++;
                        }
                     } else if (this.selected == 4 && this.lastKey != 0) {
                        this.soundOn = 1 - this.soundOn;
                     } else if ((this.lastKey & 0xFF) != 37 || this.selected != 3) {
                        if ((this.lastKey & 0xFF) == 39 && this.selected == 3 && this.startLevel < this.maxLevel && this.startLevel < 50) {
                           this.startLevel++;
                        }
                     } else if (this.startLevel > 0) {
                        this.startLevel--;
                     }
                  }

                  this.vPrint(this.menuX[this.selected], this.menuY[this.selected], this.MENUS[this.selected], this.menuColor[(this.counter & 4) >> 2]);
                  this.gg.setColor(new Color(160, 0, 0));
                  this.gg.fillRect(this.menuX[3] + this.fm.stringWidth(this.MENUS[3]), this.menuY[3], 40, 15);
                  this.gg.fillRect(this.menuX[4] + this.fm.stringWidth(this.MENUS[4]), this.menuY[4], 40, 15);
                  this.LEVELTEXT[6] = (char)(48 + (this.startLevel + 1) / 10 % 10);
                  this.LEVELTEXT[7] = (char)(48 + (this.startLevel + 1) % 10);
                  this.vPrint(this.menuX[4] + this.fm.stringWidth(this.MENUS[4]) + 7, this.menuY[4], this.MENUS[6 + this.soundOn], Color.white);
                  this.vPrint(this.menuX[3] + this.fm.stringWidth(this.MENUS[3]) + 7, this.menuY[3], this.LEVELTEXT, 6, 2);
                  if (((this.lastKey & 0xFF) == 10 || (this.lastKey & 0xFF) == 32) && this.selected == 5) {
                     this.buildShadows((byte)12);
                     this.storePermanent();
                     this.prepareIntro(200);
                  }
            }

            this.repaint();
            long var10 = System.currentTimeMillis() - var8;
            if (var10 < 65L) {
               Thread.sleep(75L - var10);
            } else {
               Thread.sleep(10L);
            }
         }
      } catch (InterruptedException var13) {
      }
   }

   public void update(Graphics var1) {
      var1.drawImage(this.background, 0, 0, this);
   }

   public void paint(Graphics var1) {
      var1.drawImage(this.background, 0, 0, this);
   }

   byte[] loadPixels(String var1, int var2) {
      byte[] var3 = null;

      try {
         InputStream var4 = this.getClass().getResourceAsStream(var1);
         var3 = new byte[var2];

         for (int var5 = 0; var5 < var2; var5++) {
            var3[var5] = (byte)var4.read();
         }

         var4.close();
      } catch (Exception var6) {
         var6.printStackTrace();
      }

      return var3;
   }

   void loadLevelGraphics(int var1) {
      byte[] var6 = this.loadPixels(rawFolder + this.levNames[var1], this.levLengths[var1]);

      for (int var2 = 0; var2 < 768; var2++) {
         this.backByte[var2 + 3072] = var6[var2];
         this.backByte[var2 + 3840] = var6[var2];
      }

      for (int var9 = 0; var9 < 384; var9++) {
         this.backByte[var9 + 4224] = var6[768 + var9];
      }

      int var5 = 4608;

      for (int var4 = 0; var4 < 16; var4++) {
         for (int var10 = 0; var10 < 3; var10++) {
            for (int var3 = 0; var3 < 24; var3++) {
               this.backByte[var5++] = var6[1152 + var4 * 24 + var10 * 384 + var3];
            }
         }
      }

      this.levGround = (this.levLengths[var1] - 2304) / 192;
      int var13 = this.levGround * 192;
      int var14 = 5760;
      var5 = 2304;

      for (int var11 = 0; var11 < var13; var11++) {
         this.backByte[var14++] = var6[var5++];
      }

      for (int var12 = 4; var12 < 17; var12++) {
         this.props[var12].flush();
      }

      try {
         this.mt.waitForID(1);
      } catch (InterruptedException var8) {
      }
   }

   void updateScore(int var1) {
      int var2 = 4;
      this.score += var1;

      for (int var3 = this.score; var2 >= 0; var2--) {
         this.scString[var2] = (char)(48 + var3 % 10);
         var3 /= 10;
      }
   }

   void drawSprite(iceObject var1) {
      int var6 = 24 - var1.x % 24;
      int var2 = var1.y - this.spriteEffH[var1.look] + 48;
      int var4 = var1.x / 24 + 12 * (var1.y >> 4);
      int var5 = var4 + 1;

      int var3;
      for (var3 = (var1.y & 240) + 16; var4 < 168 && (this.map[var4] & 3) == 0 && var3 < var2 + 32; var4 += 12) {
         var3 += 16;
      }

      if (var3 > var2) {
         var3 -= var2;
         if (var3 > this.spriteH[var1.look]) {
            var3 = this.spriteH[var1.look];
         }

         this.gg.setClip(var1.x - this.offsX, var1.y - this.offsY - this.spriteEffH[var1.look] + 48, var6, var3);
         this.gg.drawImage(this.sprites[var1.look], var1.x - this.offsX, var1.y - this.offsY - this.spriteEffH[var1.look] + 48, this);
      }

      if (var6 != 24) {
         var2 = var1.y - this.spriteEffH[var1.look] + 48;

         for (var3 = (var1.y & 240) + 16; var5 < 168 && (this.map[var5] & 3) == 0 && var3 < var2 + 32; var5 += 12) {
            var3 += 16;
         }

         if (var3 > var2) {
            var3 -= var2;
            if (var3 > this.spriteH[var1.look]) {
               var3 = this.spriteH[var1.look];
            }

            this.gg.setClip(var1.x - this.offsX + var6, var1.y - this.offsY - this.spriteEffH[var1.look] + 48, 24 - var6, var3);
            this.gg.drawImage(this.sprites[var1.look], var1.x - this.offsX, var1.y - this.offsY - this.spriteEffH[var1.look] + 48, this);
         }
      }

      this.gg.setClip(0, 0, 176, 208);
   }

   void drawShadow(iceObject var1) {
      int var3 = 0;
      if (this.shadowX[var1.look] > 0) {
         if (this.shadowX[var1.look] == 8) {
            var3 = 2;
         }

         int var4 = var1.y + 32;
         int var9 = var1.x + this.shadowX[var1.look];
         if ((var9 + var1.y + this.counter & 1) > 0) {
            var3++;
         }

         int var6 = 24 - var9 % 24;
         int var7 = var9 / 24 + 12 * (var1.y >> 4);
         int var8 = var7 + 1;

         int var2;
         for (var2 = (var1.y & 240) + 16; var7 < 168 && (this.map[var7] & 3) == 0 && var2 < var4 + 16; var7 += 12) {
            var2 += 16;
         }

         if (var2 > var4) {
            var2 -= var4;
            if (var2 > 16) {
               var2 = 16;
            }

            this.gg.setClip(var9 - this.offsX, var4 - this.offsY, var6, var2);
            this.gg.drawImage(this.shadows[var3], var9 - this.offsX, var4 - this.offsY, this);
         }

         if (var6 != 24) {
            int var5;
            if ((this.map[var8] & 3) != 0) {
               var5 = 16 - (var1.y & 15);
            } else {
               var5 = 0;
            }

            var2 = (var1.y & 240) + 32;

            for (int var14 = var8 + 12; var14 < 168 && (this.map[var14] & 3) == 0 && var2 < var4 + 16; var14 += 12) {
               var2 += 16;
            }

            if (var2 > var4 + var5) {
               var2 -= var4;
               if (var2 > 16) {
                  var2 = 16;
               }

               var2 -= var5;
               if (var2 > 0) {
                  this.gg.setClip(var9 + var6 - this.offsX, var4 + var5 - this.offsY, 24 - var6, var2);
                  this.gg.drawImage(this.shadows[var3], var9 - this.offsX, var4 - this.offsY, this);
               }
            }
         }

         this.gg.setClip(0, 0, 176, 208);
      }
   }

   void addBlock(int var1, int var2) {
      int var4 = var1 / 12;
      int var3 = var1 % 12;
      this.map[var1] = var2;
      int var5 = 2 * var2 - 2;
      if (this.map[var1 + 11] != 0 && this.map[var1 + 12] == 0) {
         var5++;
      }

      byte var6 = 16;
      if (this.map[var1 + 12] == 0) {
         var6 = 32;
      }

      this.bgg.setClip(var3 * 24, 16 + var4 * 16, 24, var6);
      this.bgg.drawImage(this.props[var5], var3 * 24, 16 + var4 * 16, this);
      if (this.map[var1 + 1] == 0) {
         if (this.map[var1 - 11] != 0) {
            var5 = 2 * this.map[var1 - 11] - 1;
            var6 = 16;
            if (this.map[var1 + 1] == 0) {
               var6 = 32;
            }

            this.bgg.setClip(24 + var3 * 24, var4 * 16, 24, var6);
            this.bgg.drawImage(this.props[var5], 24 + var3 * 24, var4 * 16, this);
         }

         if (this.map[var1 + 13] == 0) {
            this.bgg.setClip(24 + var3 * 24, 32 + var4 * 16, 12, 16);
            var5 = this.ground[var1 * 4 + 4] * 2 + 8;
            this.bgg.drawImage(this.props[var5], 24 + var3 * 24, 32 + var4 * 16, this);
            var5 = this.ground[var1 * 4 + 6] * 2 + 8;
            this.bgg.drawImage(this.props[var5], 24 + var3 * 24, 40 + var4 * 16, this);
         }
      }

      this.bgg.setClip(0, 0, 276, 248);
   }

   void removeBlock(int var1) {
      int var3 = var1 / 12;
      int var2 = var1 % 12;
      if ((this.map[var1 + 1] & 3) == 0) {
         if ((this.map[var1 - 11] & 3) != 0) {
            int var5 = 2 * this.map[var1 - 11] - 2;
            byte var4 = 16;
            if (this.map[var1 + 1] == 0) {
               var4 = 32;
            }

            this.bgg.setClip(24 + var2 * 24, var3 * 16, 24, var4);
            this.bgg.drawImage(this.props[var5], 24 + var2 * 24, var3 * 16, this);
         }

         if ((this.map[var1 + 13] & 3) == 0) {
            this.bgg.setClip(24 + var2 * 24, 32 + var3 * 16, 12, 16);
            int var6 = this.ground[var1 * 4 + 4] * 2 + 7;
            this.bgg.drawImage(this.props[var6], 24 + var2 * 24, 32 + var3 * 16, this);
            var6 = this.ground[var1 * 4 + 6] * 2 + 7;
            this.bgg.drawImage(this.props[var6], 24 + var2 * 24, 40 + var3 * 16, this);
         }
      }

      this.bgg.setClip(var2 * 24, 16 + var3 * 16, 24, 32);
      if ((this.map[var1 + 12] & 3) == 0) {
         int var8 = this.ground[var1 * 4] * 2 + 7;
         if ((this.map[var1 - 1] & 3) != 0) {
            var8++;
         }

         this.bgg.drawImage(this.props[var8], var2 * 24, 32 + var3 * 16, this);
         var8 = this.ground[var1 * 4 + 2] * 2 + 7;
         if ((this.map[var1 - 1] & 3) != 0) {
            var8++;
         }

         this.bgg.drawImage(this.props[var8], var2 * 24, 40 + var3 * 16, this);
         var8 = this.ground[var1 * 4 + 1] * 2 + 7;
         this.bgg.drawImage(this.props[var8], 12 + var2 * 24, 32 + var3 * 16, this);
         var8 = this.ground[var1 * 4 + 3] * 2 + 7;
         this.bgg.drawImage(this.props[var8], 12 + var2 * 24, 40 + var3 * 16, this);
      }

      if ((this.map[var1 - 12] & 3) != 0) {
         int var12 = 2 * (this.map[var1 - 12] & 3) - 2;
         if ((this.map[var1 - 1] & 3) != 0) {
            var12++;
         }

         this.bgg.drawImage(this.props[var12], var2 * 24, var3 * 16, this);
      } else {
         int var13 = this.ground[var1 * 4 - 48] * 2 + 7;
         if ((this.map[var1 - 13] & 3) != 0) {
            var13++;
         }

         this.bgg.drawImage(this.props[var13], var2 * 24, 16 + var3 * 16, this);
         var13 = this.ground[var1 * 4 - 46] * 2 + 7;
         if ((this.map[var1 - 13] & 3) != 0) {
            var13++;
         }

         this.bgg.drawImage(this.props[var13], var2 * 24, 24 + var3 * 16, this);
         var13 = this.ground[var1 * 4 - 47] * 2 + 7;
         this.bgg.drawImage(this.props[var13], 12 + var2 * 24, 16 + var3 * 16, this);
         var13 = this.ground[var1 * 4 - 45] * 2 + 7;
         this.bgg.drawImage(this.props[var13], 12 + var2 * 24, 24 + var3 * 16, this);
      }

      this.bgg.setClip(0, 0, 276, 248);
   }

   void sortSprites() {
      this.sortN = 0;

      for (int var1 = 0; var1 < 10; var1++) {
         if (this.objs[var1].type > 0) {
            this.sortY[this.sortN] = this.objs[var1].y;
            if (this.objs[var1].type == 8) {
               this.sortY[this.sortN] = this.sortY[this.sortN] + 100;
            }

            this.sortIX[this.sortN++] = var1;
         }
      }

      for (int var2 = 1; var2 < this.sortN; var2++) {
         for (int var4 = 0; var4 < var2; var4++) {
            if (this.sortY[var4] > this.sortY[var2]) {
               int var3 = this.sortY[var2];
               this.sortY[var2] = this.sortY[var4];
               this.sortY[var4] = var3;
               var3 = this.sortIX[var2];
               this.sortIX[var2] = this.sortIX[var4];
               this.sortIX[var4] = var3;
            }
         }
      }
   }

   void prepareEnemies() {
      int var1 = 0;
      int var2 = 0;
      int var3;
      if (this.level > 19) {
         var3 = 19;
      } else {
         var3 = this.level;
      }

      while (var1 < this.levFl1[var3]) {
         this.enemies[var2++] = 6;
         var1++;
      }

      for (int var4 = 0; var4 < this.levFl2[var3]; var4++) {
         this.enemies[var2++] = 7;
      }

      while (var2 < 4) {
         this.enemies[var2++] = 0;
      }
   }

   void prepareLevel() {
      int[] var1 = new int[]{0, 9, 5, 7};
      if (this.level < 20) {
         var1[1] = this.levIce[this.level];
         var1[3] = this.levRock[this.level];
      }

      this.buildMap(var1);
      this.loadLevelGraphics(this.level / 2 % 10);
      this.buildTiles();
      this.offsX = 12;
      this.offsY = 0;

      for (int var2 = 0; var2 < 10; var2++) {
         this.objs[var2].type = 0;
      }

      this.addObject(1, 13, 11, 0);
      this.coins = 0;
   }

   void startSession() {
      this.gameMode = 3;
      this.level = this.startLevel;
      this.lives = 2;
      this.score = 0;
      this.updateScore(0);
      this.clipX = 80;
      this.clipW = 16;
      this.clipY = 100;
      this.clipH = 8;
      this.buildShadows((byte)0);
   }

   void prepareSettings() {
      this.gg.setColor(new Color(160, 0, 0));
      this.gg.fillRect(0, 0, 176, 208);
      this.prepareMenu(3);
      this.vPrint(88 - this.fm.stringWidth(this.MENUS[1]) / 2, 17, this.MENUS[1], Color.white);
      this.gameMode = 10;
      this.objs[0].x = 76;
      this.objs[1].x = 24;
      this.objs[2].x = 128;
      this.objs[0].y = 180;
      this.objs[1].y = 180;
      this.objs[2].y = 180;
      this.objs[0].look = 6;
      this.objs[1].look = 18;
      this.objs[2].look = 25;
      this.buildShadows((byte)96);

      for (int var1 = 0; var1 < 3; var1++) {
         this.drawShadowSimple(var1);
         this.drawSpriteSimple(var1);
      }

      if (this.certified == 0) {
         this.vPrint(0, 0, this.VARIOUS[2], Color.white);
      }
   }

   void prepareHighscores() {
      int var1;
      for (var1 = 7; var1 > 0 && this.score > this.highscores[var1 - 1]; var1--) {
         for (int var2 = 3; var2 < 14; var2++) {
            this.HIGHSCORES[var1][var2] = this.HIGHSCORES[var1 - 1][var2];
         }

         this.highscores[var1] = this.highscores[var1 - 1];
      }

      this.highscores[var1] = this.score;

      for (int var4 = 0; var4 < 5; var4++) {
         this.HIGHSCORES[var1][4 + var4] = this.scString[var4];
      }

      for (int var5 = 0; var5 < 3; var5++) {
         this.HIGHSCORES[var1][11 + var5] = '.';
      }

      this.highRow = var1;
      this.highCol = 11;
      this.gg.setColor(new Color(0, 0, 171));
      this.gg.fillRect(0, 0, 176, 208);
      this.vPrint(88 - this.fm.stringWidth(this.VARIOUS[0]) / 2, 10, this.VARIOUS[0], Color.white);

      for (int var3 = 0; var3 < 8; var3++) {
         this.vPrint(this.scoreX, 36 + var3 * 15, this.HIGHSCORES[var3], 0, 14);
      }

      this.objs[0].x = 10;
      this.objs[0].y = 184;
      this.objs[0].look = 0;
      this.buildShadows((byte)1);
      this.drawShadowSimple(0);
      this.drawSpriteSimple(0);
      this.gg.drawImage(this.sprites[47], 156, this.highRow * 15 + 39, this);
      this.gameMode = 9;
   }

   public void buildTiles() {
      for (int var1 = 0; var1 < 672; var1++) {
         this.ground[var1] = 0;
      }

      for (int var5 = 1; var5 < this.levGround; var5++) {
         for (int var2 = 0; var2 < 44; var2++) {
            this.ground[48 + (this.rnd.nextInt() & 65535) % 576] = (byte)var5;
         }
      }

      for (int var6 = 0; var6 < 14; var6++) {
         for (int var8 = 0; var8 < 12; var8++) {
            int var3 = 12 * var6 + var8;
            if (this.map[var3] != 0) {
               int var4 = 2 * this.map[var3] - 2;
               if (var8 > 0 && var6 < 13 && this.map[var3 + 11] != 0) {
                  var4++;
               }

               this.bgg.drawImage(this.props[var4], var8 * 24, 16 + var6 * 16, this);
            } else {
               int var9 = 7 + 2 * this.ground[var3 * 4];
               if (var8 > 0 && this.map[var3 - 1] != 0) {
                  var9++;
               }

               this.bgg.drawImage(this.props[var9], var8 * 24, 32 + var6 * 16, this);
               var9 = 7 + 2 * this.ground[var3 * 4 + 1];
               this.bgg.drawImage(this.props[var9], var8 * 24 + 12, 32 + var6 * 16, this);
               var9 = 7 + 2 * this.ground[var3 * 4 + 2];
               if (var8 > 0 && this.map[var3 - 1] != 0) {
                  var9++;
               }

               this.bgg.drawImage(this.props[var9], var8 * 24, 40 + var6 * 16, this);
               var9 = 7 + 2 * this.ground[var3 * 4 + 3];
               this.bgg.drawImage(this.props[var9], var8 * 24 + 12, 40 + var6 * 16, this);
            }
         }
      }

      for (int var7 = 0; var7 < 4; var7++) {
         this.bgg.drawImage(this.props[6], var7 * 72, 0, this);
      }
   }

   public IcePlus() {
      // Initialize your components here
      // Set up the JFrame properties

      setDefaultCloseOperation(EXIT_ON_CLOSE);
      setResizable(false); // Make the window non-scalable
      setLocationRelativeTo(null);
      setUndecorated(true); 
      // Set the preferred size of the content pane
      getContentPane().setPreferredSize(new Dimension(176, 208));

      // Add any components you had in your Applet
      // Add mouse and key listeners
      addMouseListener(this);
      addKeyListener(this);
      // Set title if needed
      setTitle("IcePlus");
      pack();
      // Get the graphics context of the content pane;
      init();
   }

   public static void main(String[] args) {
      SwingUtilities.invokeLater(new Runnable() {
         public void run() {
               IcePlus app = new IcePlus();
               app.setVisible(true);
         }
      });
   }

   public void keyTyped(KeyEvent var1) {
   }

   public void keyPressed(KeyEvent var1) {
      this.lastKey = var1.getKeyCode() + var1.getKeyChar() * 256;
   }

   public void keyReleased(KeyEvent var1) {
      this.lastKey = 0;
   }

   public void mouseClicked(MouseEvent var1) {
   }

   public void mouseEntered(MouseEvent var1) {
   }

   public void mouseExited(MouseEvent var1) {
   }

   public void mousePressed(MouseEvent var1) {
      this.requestFocus();
   }

   public void mouseReleased(MouseEvent var1) {
   }
}
