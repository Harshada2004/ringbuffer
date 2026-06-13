#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE 8U

typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} RingBuffer;

void RingBuffer_Init(RingBuffer *rb)
{
    rb->head = 0U;
    rb->tail = 0U;
    rb->count = 0U;
}

uint8_t RingBuffer_IsEmpty(const RingBuffer *rb)
{
    return (rb->count == 0U);
}

uint8_t RingBuffer_IsFull(const RingBuffer *rb)
{
    return (rb->count == BUFFER_SIZE);
}

uint8_t RingBuffer_Count(const RingBuffer *rb)
{
    return rb->count;
}

int RingBuffer_Write(RingBuffer *rb, uint8_t data)
{
    if (RingBuffer_IsFull(rb))
    {
        return -1;
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1U) & (BUFFER_SIZE - 1U);
    rb->count++;

    return 0;
}

int RingBuffer_Read(RingBuffer *rb, uint8_t *data)
{
    if (RingBuffer_IsEmpty(rb))
    {
        return -1;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1U) & (BUFFER_SIZE - 1U);
    rb->count--;

    return 0;
}

int main(void)
{
    RingBuffer rb;
    uint8_t data;
    uint8_t i;

    RingBuffer_Init(&rb);

    uint8_t first_data[8] =
    {
        0x41, 0x42, 0x43, 0x44,
        0x45, 0x46, 0x47, 0x48
    };

    for (i = 0U; i < 8U; i++)
    {
        if (RingBuffer_Write(&rb, first_data[i]) == 0)
        {
            printf("[WRITE] 0x%02X -> OK (count=%u)",
                   first_data[i],
                   RingBuffer_Count(&rb));

            if (RingBuffer_IsFull(&rb))
            {
                printf(" FULL");
            }

            printf("\n");
        }
    }

    if (RingBuffer_Write(&rb, 0x99) != 0)
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    for (i = 0U; i < 3U; i++)
    {
        if (RingBuffer_Read(&rb, &data) == 0)
        {
            printf("[READ] -> 0x%02X (count=%u)\n",
                   data,
                   RingBuffer_Count(&rb));
        }
    }

    uint8_t second_data[3] = {0x49, 0x4A, 0x4B};

    for (i = 0U; i < 3U; i++)
    {
        if (RingBuffer_Write(&rb, second_data[i]) == 0)
        {
            printf("[WRITE] 0x%02X -> OK (count=%u)\n",
                   second_data[i],
                   RingBuffer_Count(&rb));
        }
    }

    while (!RingBuffer_IsEmpty(&rb))
    {
        if (RingBuffer_Read(&rb, &data) == 0)
        {
            printf("[READ] -> 0x%02X (count=%u)\n",
                   data,
                   RingBuffer_Count(&rb));
        }
    }

    printf("Buffer Empty = YES\n");

    if (RingBuffer_Read(&rb, &data) != 0)
    {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}