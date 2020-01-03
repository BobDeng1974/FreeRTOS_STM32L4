#include "mcu_init.h"
#include "os_logo.h"

#define START_TASK_PRIO 1            // 任务优先级
#define START_STK_SIZE 512           // 任务堆栈大小
TaskHandle_t Start_Task_Handler;     // 任务句柄
void start_task(void *pvParameters); // 任务函数

#define TASK_TASK1_PRIO 3            // 任务优先级
#define TASK1_STK_SIZE 512           // 任务堆栈大小
TaskHandle_t TASK1_Task_Handler;     // 任务句柄
void task_task1(void *pvParameters); // 任务函数

#define TASK_TASK2_PRIO 3            // 任务优先级
#define TASK2_STK_SIZE 512           // 任务堆栈大小
TaskHandle_t TASK2_Task_Handler;     // 任务句柄
void task_task2(void *pvParameters); // 任务函数

#define FLOAT_TASK_PRIO 4            // 任务优先级
#define FLOAT_STK_SIZE 1024          // 任务堆栈大小
TaskHandle_t FLOAT_Task_Handler;     // 任务句柄
void float_task(void *pvParameters); // 任务函数

static uint32_t task1_arg_array1[3] = {1, 2, 3}; // 任务入口函数参数
static uint32_t task1_arg_array2[3] = {4, 5, 6}; // 任务入口函数参数
static char *task2_arg_str = "Hello FreeRTOS";  // 任务入口函数参数
/********************************************************/
//LCD刷屏时使用的颜色
int lcd_discolor[13] = {WHITE, BLUE, BRED, GRED, GBLUE, RED, MAGENTA,
                        GREEN, CYAN, YELLOW, BROWN, BRRED, GRAY};

/********************************************************/

int main(void)
{
    /* 初始化硬件 */
    board_init();
    LCD_DisplayOn();
    LCD_Show_Image(10, 6, 220, 84, os_logo);

    BACK_COLOR = WHITE;
    POINT_COLOR = BLUE;
    LCD_ShowString(10, 95, 240, 24, 24, "Pandora STM32L475");

    POINT_COLOR = RED;
    LCD_ShowString(10, 120, 240, 16, 16, "QinYUN575"
                                         "@" __DATE__);

    printf("\r\n===========================================================\r\n");
    printf("\r\n");
    printf("\tWelcome to FreeRTOS\r\n");
    printf("\tCompile Time:%s, %s\r\n", __TIME__, __DATE__);
    printf("\tCoding by QinYUN575\r\n");
    printf("\r\n");
    printf("===========================================================\r\n");
    printf("[%d:%s]: start\r\n", __LINE__, __FUNCTION__);
    //创建开始任务
    xTaskCreate((TaskFunction_t)start_task,           //任务函数
                (const char *)"start_task",           //任务名称
                (uint16_t)START_STK_SIZE,             //任务堆栈大小
                (void *)NULL,                         //传递给任务函数的参数
                (UBaseType_t)START_TASK_PRIO,         //任务优先级
                (TaskHandle_t *)&Start_Task_Handler); //任务句柄
    vTaskStartScheduler();                            //开启任务调度
}

/******************************************************************************/

//开始任务任务函数
void start_task(void *pvParameters)
{
    const char* START_TASK_TAG = "START";
    LOGI(START_TASK_TAG, "[%d:%s]: start", __LINE__, __FUNCTION__);
    taskENTER_CRITICAL(); //进入临界区
    // 创建TASK1任务
    LOGI(START_TASK_TAG, "Create Task1");
    xTaskCreate((TaskFunction_t)task_task1,
                (const char *)"task_task1",
                (uint16_t)TASK1_STK_SIZE,
                (void *)task1_arg_array1,
                (UBaseType_t)TASK_TASK1_PRIO,
                (TaskHandle_t *)&TASK1_Task_Handler);

    // 创建TASK2任务
    LOGI(START_TASK_TAG, "Create Task2");
    xTaskCreate((TaskFunction_t)task_task2,
                (const char *)"task_task2",
                (uint16_t)TASK2_STK_SIZE,
                (void *)task2_arg_str,
                (UBaseType_t)TASK_TASK2_PRIO,
                (TaskHandle_t *)&TASK2_Task_Handler);

    // 浮点测试任务
    LOGI(START_TASK_TAG, "Create float Task");
    xTaskCreate((TaskFunction_t)float_task,
                (const char *)"float_task",
                (uint16_t)FLOAT_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)FLOAT_TASK_PRIO,
                (TaskHandle_t *)&FLOAT_Task_Handler);

    LOGI(START_TASK_TAG, "Destory Start Task");
    vTaskDelete(Start_Task_Handler); //删除开始任务
    taskEXIT_CRITICAL();             //退出临界区
}

void dump_uint32_array(uint32_t *array, size_t array_len)
{
    printf("\r\n----------\r\n");
    printf("dump uint32 data:\r\n");
    for (int i = 0; i < array_len; i++)
    {
        printf("%5d\t", array[i]);
        if (i >= 10)
        {
            printf("\r\n");
        }
    }
    printf("\r\n----------\r\n");
}

void task_task1(void *pvParameters)
{
    const char* TASK1_TAG = "TASK1";
    taskENTER_CRITICAL(); //进入临界区
    uint32_t *array = (uint32_t *)pvParameters;

    LOGI(TASK1_TAG, "[%d:%s]: start", __LINE__, __FUNCTION__);
    dump_uint32_array(array, 3);

    uint8_t task1_count = 0;
    POINT_COLOR = BLUE;
    LCD_ShowString(10, 140, 84, 12, 12, "Task1 Run: 000");
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(10, 155, 80, 225);
    taskEXIT_CRITICAL(); //退出临界区
    for (;;)
    {
        taskENTER_CRITICAL(); //进入临界区
        LCD_Fill(11, 156, 79, 224, lcd_discolor[task1_count++ % 13]);
        LCD_ShowNum(76, 140, task1_count, 3, 12);
        HAL_GPIO_TogglePin(GPIOE, LED_R_Pin);

        LOGI(TASK1_TAG, "runing...");
        taskEXIT_CRITICAL(); //退出临界区
        vTaskDelay(350);
    }
}

void task_task2(void *pvParameters)
{
    const char* TASK2_TAG = "TASK2";
    taskENTER_CRITICAL(); //进入临界区
    int i = 0;
    char *task2_arg = (char *)pvParameters;
    LOGI(TASK2_TAG,"[%d:%s]: start", __LINE__, __FUNCTION__);
    LOGI(TASK2_TAG,"task2_arg:%s", task2_arg);

    uint8_t task2_count = 0;
    POINT_COLOR = BLUE;
    LCD_ShowString(125, 140, 84, 12, 12, "Task2 Run: 000");
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(125, 155, 195, 225);
    taskEXIT_CRITICAL(); //退出临界区
    for (;;)
    {
        taskENTER_CRITICAL(); //进入临界区
        LCD_Fill(126, 156, 194, 224, lcd_discolor[task2_count++ % 13]);
        LCD_ShowNum(191, 140, task2_count, 3, 12);
        HAL_GPIO_TogglePin(GPIOE, LED_G_Pin);
        LOGI(TASK2_TAG,"i = :%d", i);
        taskEXIT_CRITICAL(); //退出临界区

        switch (i)
        {
        case 5:
            /* 挂起 task1 */
            LOGW(TASK2_TAG,"[%d:%s]: Suspend Task1", __LINE__, __FUNCTION__);
            vTaskSuspend(TASK1_Task_Handler);
            break;

        case 15:
            /* 恢复挂起 task1 */
            LOGW(TASK2_TAG,"[%d:%s]: Resume Task1", __LINE__, __FUNCTION__);
            vTaskResume(TASK1_Task_Handler);
            break;

        case 20:
            /* 销毁 task1 */
            LOGW(TASK2_TAG,"[%d:%s]: Destory Task1", __LINE__, __FUNCTION__);
            vTaskDelete(TASK1_Task_Handler);
            break;

        case 30:
            /* 重新创建 task1 */
            LOGW(TASK2_TAG,"[%d:%s]: Recreate Task1", __LINE__, __FUNCTION__);
            xTaskCreate((TaskFunction_t)task_task1,
            (const char *)"task_task1_2",
            (uint16_t)TASK1_STK_SIZE,
            (void *)task1_arg_array2,
            (UBaseType_t)TASK_TASK1_PRIO,
            (TaskHandle_t *)&TASK1_Task_Handler);
            break;

        case 35:
            /* 更改 task1 优先级 */
            LOGW(TASK2_TAG,"[%d:%s]: change Task1 priority", __LINE__, __FUNCTION__);
            vTaskPrioritySet(TASK1_Task_Handler, TASK_TASK1_PRIO - 1);
            break;

        case 50:

            HAL_GPIO_WritePin(GPIOE, LED_R_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOE, LED_G_Pin, GPIO_PIN_SET);
            /* 销毁 task1,task2 */
            //LOGW(TASK2_TAG,"[%d:%s]: Destory Task1", __LINE__, __FUNCTION__);
            //vTaskDelete(TASK1_Task_Handler);
            LOGW(TASK2_TAG,"[%d:%s]: Destory Task2", __LINE__, __FUNCTION__);
            vTaskDelete(TASK2_Task_Handler);
            break;

        default:
            break;
        }
        LOGI(TASK2_TAG,"runing...");
        vTaskDelay(1000);
        i++;
    }
}

//浮点测试任务
void float_task(void *pvParameters)
{
    const char* TASK_fLOAT_TAG = "TASK_fLOAT";
    static float float_num = 0.00;
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOE, LED_B_Pin);
        float_num += 0.001f;
        LOGI(TASK_fLOAT_TAG, "float_num: %.4f", float_num);
        vTaskDelay(1500);
    }
}
