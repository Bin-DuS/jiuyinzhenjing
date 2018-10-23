/**		
 * @file    	sms4.h
 * @date: 		2016/03/09	
 * @brief  	    SMS4�ӽ���ͷ�ļ�����������������
 *              1. sms4_set_key
 *                  ����KEY
 *              2. sms4_encrypt
 *                  ����
 *              3. sms4_decrypt
 *                  ����
 * @author:		xhtian
 * �޸�������	001)[�޸�ԭ��1]		
 * �޸����ߣ�	001)�޸��� YYYY/MM/DD		
 * @version   	1.0
 * Copyright(c) All Right Reserved, Copyright(c) 2016.		
 */	
#ifndef __SMS4_H__
#define __SMS4_H__

/**
 * @brief ������Կ
 * 		@param [in/out] unsigned int * key ��Կ������128bits
 * @return void ��
 * @note  
 * @see   
 */
void sms4_set_key(unsigned int* key);

/**
 * @brief SMS4����
 * 		@param [in/out] char * in ��������
 * 		@param [in/out] int in_len �������ݳ��ȣ����ֽڼ�
 * 		@param [in/out] char * out �������
 * 		@param [in/out] int out_len ������ݳ��ȣ����ֽڼ�
 * @return void ��
 * @note  ��128bitsΪһ�����ܵ�Ԫ��ʣ�಻��128bits�����ݲ�����
 * @see   
 */
void sms4_encrypt(char* in, int in_len, char* out,int out_len);

/**
 * @brief SMS4����
 * 		@param [in/out] char * in ��������
 * 		@param [in/out] int in_len �������ݳ��ȣ����ֽڼ�
 * 		@param [in/out] char * out �������
 * 		@param [in/out] int out_len ������ݳ��ȣ����ֽڼ�
 * @return void ��
 * @note  ��128bitsΪһ�����ܵ�Ԫ��ʣ�಻��128bits�����ݲ�����
 * @see   
 */
void sms4_decrypt(char* in, int in_len, char* out,int out_len);

// �������ݼ���
void sms4_data_encrypt(
    char* input, 
    unsigned int input_len, 
    char* output, 
    unsigned int output_len);
// �������ݽ���
void sms4_data_decrypt(
    char* input, 
    unsigned int input_len, 
    char* output, 
    unsigned int output_len);


#endif

