# �ڵ��޳�ʾ������

## Ŀ¼
 * [���](#���)
 * [����׼��](#����׼��)
 * [����Ҫ��](#����Ҫ��)
 * [���н��](#���н��)
 * [��Ȩ���](#��Ȩ���)

## ���
�ڵ��޳�ʾ����������ڵ��޳�APIʹ�÷��������������á�

## ����׼��
1. ����[cgsdk-plugin](https://developer.huawei.com/consumer/en/doc/development/HMSCore-Library-V5/sdk-download-0000001050441521-V5) ��cgsdk-plugin-occlusioncullingĿ¼��ѹ��sampleĿ¼�¡�

2. ����demo

    ��sampleĿ¼��ִ��`ndk-build NDK_PROJECT_PATH=.NDK_APPLICATION_MK=Application.mk APP_BUILD_SCRIPT=Android.mk`���

3. ����demo

    ִ��`adb push`�����"Sample\obj\local\arm64-v8a"�ļ����µ�"libpartythengine_plugin_oc.so"��"Sample"���䵽�豸��"/data/local/tmp"Ŀ¼�¡�

    ��shell��ִ��`export LD_LIBRARY_PATH=/data/local/tmp`���

    ��"/data/local/tmp"Ŀ¼��ִ��`chmod a+x Sample`���

    ��"/data/local/tmp"Ŀ¼��ִ��`./Sample`���

## ����Ҫ��
1. Android 7.0�����ϵ��豸��

## ���н��
<img src="OcclusionCullingResult.jpg" width="50%" height="50%">

## ��Ȩ���
�ڵ��޳�ʾ�������ѻ��[Apache 2.0 license](http://www.apache.org/licenses/LICENSE-2.0)��Ȩ��ɡ�
