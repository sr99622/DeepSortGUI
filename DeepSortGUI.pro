QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Cameras/admintab.cpp \
    Cameras/camera.cpp \
    Cameras/cameradialogtab.cpp \
    Cameras/cameralistmodel.cpp \
    Cameras/cameralistview.cpp \
    Cameras/camerapanel.cpp \
    Cameras/configtab.cpp \
    Cameras/discovery.cpp \
    Cameras/imagetab.cpp \
    Cameras/logindialog.cpp \
    Cameras/networktab.cpp \
    Cameras/onvifmanager.cpp \
    Cameras/ptztab.cpp \
    Cameras/videotab.cpp \
    DeepSort/errmsg.cpp \
    DeepSort/feature/model.cpp \
    DeepSort/matching/kalmanfilter.cpp \
    DeepSort/matching/linear_assignment.cpp \
    DeepSort/matching/nn_matching.cpp \
    DeepSort/matching/track.cpp \
    DeepSort/matching/tracker.cpp \
    DeepSort/thirdPart/hungarianoper.cpp \
    DeepSort/thirdPart/munkres/adapters/adapter.cpp \
    DeepSort/thirdPart/munkres/adapters/boostmatrixadapter.cpp \
    DeepSort/thirdPart/munkres/munkres.cpp \
    Ffplay/Clock.cpp \
    Ffplay/CommandOptions.cpp \
    Ffplay/Decoder.cpp \
    Ffplay/Display.cpp \
    Ffplay/EventHandler.cpp \
    Ffplay/Frame.cpp \
    Ffplay/FrameQueue.cpp \
    Ffplay/Packet.cpp \
    Ffplay/PacketQueue.cpp \
    Ffplay/VideoState.cpp \
    Ffplay/cmdutils.c \
    Filters/Tracker/featuremodel.cpp \
    Filters/Tracker/imageframe.cpp \
    Filters/Tracker/matcher.cpp \
    Filters/darknet.cpp \
    Filters/filter.cpp \
    Filters/filterchain.cpp \
    Filters/filterlistmodel.cpp \
    Filters/filterlistview.cpp \
    Filters/filterpanel.cpp \
    Filters/subpicture.cpp \
    Utilities/avexception.cpp \
    Utilities/colorbutton.cpp \
    Utilities/cudaexception.cpp \
    Utilities/directorysetter.cpp \
    Utilities/displaycontainer.cpp \
    Utilities/displayslider.cpp \
    Utilities/filepanel.cpp \
    Utilities/filesetter.cpp \
    Utilities/kalman.cpp \
    Utilities/messagedialog.cpp \
    Utilities/numbertextbox.cpp \
    Utilities/panel.cpp \
    Utilities/paneldialog.cpp \
    Utilities/waitbox.cpp \
    Utilities/yuvcolor.cpp \
    alarmpanel.cpp \
    configpanel.cpp \
    controlpanel.cpp \
    countpanel.cpp \
    main.cpp \
    mainpanel.cpp \
    mainwindow.cpp \
    optionpanel.cpp \
    parameterpanel.cpp \
    simplefilter.cpp \
    streampanel.cpp

HEADERS += \
    Cameras/admintab.h \
    Cameras/camera.h \
    Cameras/cameradialogtab.h \
    Cameras/cameralistmodel.h \
    Cameras/cameralistview.h \
    Cameras/camerapanel.h \
    Cameras/configtab.h \
    Cameras/discovery.h \
    Cameras/imagetab.h \
    Cameras/logindialog.h \
    Cameras/networktab.h \
    Cameras/onvifmanager.h \
    Cameras/ptztab.h \
    Cameras/videotab.h \
    DeepSort/errmsg.h \
    DeepSort/feature/dataType.h \
    DeepSort/feature/model.h \
    DeepSort/matching/kalmanfilter.h \
    DeepSort/matching/linear_assignment.h \
    DeepSort/matching/nn_matching.h \
    DeepSort/matching/track.h \
    DeepSort/matching/tracker.h \
    DeepSort/thirdPart/hungarianoper.h \
    DeepSort/thirdPart/munkres/adapters/adapter.h \
    DeepSort/thirdPart/munkres/adapters/boostmatrixadapter.h \
    DeepSort/thirdPart/munkres/matrix.h \
    DeepSort/thirdPart/munkres/munkres.h \
    Ffplay/Clock.h \
    Ffplay/CommandOptions.h \
    Ffplay/Decoder.h \
    Ffplay/Display.h \
    Ffplay/EventHandler.h \
    Ffplay/Frame.h \
    Ffplay/FrameQueue.h \
    Ffplay/Packet.h \
    Ffplay/PacketQueue.h \
    Ffplay/VideoState.h \
    Ffplay/cmdutils.h \
    Ffplay/config.h \
    Filters/Tracker/featuremodel.h \
    Filters/Tracker/imageframe.h \
    Filters/Tracker/matcher.h \
    Filters/darknet.h \
    Filters/filter.h \
    Filters/filterchain.h \
    Filters/filterlistmodel.h \
    Filters/filterlistview.h \
    Filters/filterpanel.h \
    Filters/subpicture.h \
    Utilities/avexception.h \
    Utilities/colorbutton.h \
    Utilities/cudaexception.h \
    Utilities/directorysetter.h \
    Utilities/displaycontainer.h \
    Utilities/displayslider.h \
    Utilities/filepanel.h \
    Utilities/filesetter.h \
    Utilities/kalman.h \
    Utilities/messagedialog.h \
    Utilities/numbertextbox.h \
    Utilities/panel.h \
    Utilities/paneldialog.h \
    Utilities/waitbox.h \
    Utilities/yuvcolor.h \
    alarmpanel.h \
    configpanel.h \
    controlpanel.h \
    countpanel.h \
    mainpanel.h \
    mainwindow.h \
    optionpanel.h \
    parameterpanel.h \
    simplefilter.h \
    streampanel.h


#CUDA_PATH = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.2"
#CONTRIB_PATH = "C:/Users/sr996/Projects/contrib

INCLUDEPATH += $$(CUDA_PATH)/include \
               $$(CONTRIB_PATH)/include/SDL \
               $$(CONTRIB_PATH)/include \
               $$(CONTRIB_PATH)/include/darknet \
               $$(CONTRIB_PATH)/include/libxml2 \
               $$(BOOST_PATH)

LIBS += -L$$(CUDA_PATH)/lib/x64 \
        -lcudart \
        -lcudnn \
        -lnppc \
        -lnpps \
        -lnppicc \
        -lnppial \
        -lnppidei \
        -lnppif \
        -lnppig \
        -lnppim \
        -lnppist \
        -lnppisu \
        -lnppitc

LIBS += -L$$(CONTRIB_PATH)/lib \
        -llibavcodec \
        -llibavformat \
        -llibavutil \
        -llibswscale \
        -llibswresample \
        -llibavdevice \
        -llibavfilter \
        -llibpostproc \
        -ldarknet \
        -llibsdl2 \
        -llibonvif \
        -lopencv_world451 \
        -ltensorflow

RESOURCES += resources/resources.qrc \
    resources/resources.qrc

DISTFILES += resources/darkstyle.qss \
             .gitignore \
             README.md \
             docs/controls.png \
             docs/controls.png \
             docs/controls_small.png \
             docs/controls_small.png \
             docs/st_pete.png \
             docs/st_pete.png \
             docs/st_pete_small.png \
             docs/st_pete_small.png \
             help/admin_tab.jpg \
             help/camera_functions.html \
             help/camera_list.jpg \
             help/configuration.html \
             help/configuration_tab.jpg \
             help/diff.jpg \
             help/diffxy.jpg \
             help/filters.html \
             help/filters_tab.jpg \
             help/gif_writer.jpg \
             help/image_tab.jpg \
             help/network_tab.jpg \
             help/pictures.html \
             help/pictures_file_tab.jpg \
             help/ptz_tab.jpg \
             help/sub_picture.jpg \
             help/system_overview.html \
             help/temp.png \
             help/tips.html \
             help/user_manual.html \
             help/video_file_tab.jpg \
             help/video_tab.jpg \
             help/videos.html \
             icon/playqt.ico \
             icon/playqt_16.png \
             icon/playqt_256.png \
             icon/playqt_32.png \
             icon/playqt_48.png \
             playqt.ico \
             playqt.ico \
             resources/audio.png \
             resources/audio_hi.png \
             resources/audio_lo.png \
             resources/branch_closed.png \
             resources/branch_closed_hi.png \
             resources/branch_closed_lo.png \
             resources/branch_open.png \
             resources/branch_open_hi.png \
             resources/branch_open_lo.png \
             resources/checked.png \
             resources/checked_hi.png \
             resources/checked_lo.png \
             resources/darkstyle.qss \
             resources/fast-forward.png \
             resources/fast-forward_hi.png \
             resources/fast-forward_lo.png \
             resources/mute.png \
             resources/mute_hi.png \
             resources/mute_lo.png \
             resources/next.png \
             resources/next_hi.png \
             resources/next_lo.png \
             resources/pause.png \
             resources/pause_hi.png \
             resources/pause_lo.png \
             resources/play.png \
             resources/play_hi.png \
             resources/play_lo.png \
             resources/playqt_256.png \
             resources/previous.png \
             resources/previous_hi.png \
             resources/previous_lo.png \
             resources/radio-off.png \
             resources/radio-off_hi.png \
             resources/radio-off_lo.png \
             resources/radio-on.png \
             resources/radio-on_hi.png \
             resources/radio-on_lo.png \
             resources/resources.rcc \
             resources/resources.rcc \
             resources/resources_bak.rcc \
             resources/rewind.png \
             resources/rewind_hi.png \
             resources/rewind_lo.png \
             resources/small_arrow_left.png \
             resources/small_arrow_left_hi.png \
             resources/small_arrow_left_lo.png \
             resources/small_arrow_up.png \
             resources/small_arrow_up_hi.png \
             resources/small_arrow_up_lo.png \
             resources/stop.png \
             resources/stop_hi.png \
             resources/stop_lo.png \
             resources/taurus.gif \
             resources/unchecked.png \
             resources/unchecked_hi.png \
             resources/unchecked_lo.png

RC_ICONS = playqt.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
