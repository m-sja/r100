#include <cogl/cogl.h>  
#include <glib.h>  
#include <stdio.h>  
  
typedef struct _Data  
{  
    CoglContext *ctx;  
    CoglFramebuffer *fb;  
    CoglPrimitive *triangle;  
    CoglPipeline *pipeline;  
  
    unsigned int redraw_idle;  
    CoglBool frame_sync_ready;  
    CoglBool is_dirty;  
} Data;  
  
static CoglBool  
paint_cb (void *user_data)  
{  
    Data *data = user_data;  
  
    data->redraw_idle = 0;  
    data->is_dirty = FALSE;  
    data->frame_sync_ready = FALSE;  
  
    cogl_framebuffer_clear4f (data->fb, COGL_BUFFER_BIT_COLOR, 0, 0, 0, 1);  
    cogl_primitive_draw (data->triangle, data->fb, data->pipeline);  
    cogl_onscreen_swap_buffers (data->fb);  
  
    return FALSE; /* un-register idle callback */  
}  
  
static void  
maybe_queue_redraw (Data *data)  
{  
    if (data->is_dirty && data->frame_sync_ready && data->redraw_idle == 0) {  
        /* We'll draw on idle instead of drawing immediately so that 
         * if Cogl reports multiple dirty rectangles we won't 
         * redundantly draw multiple frames */  
        data->redraw_idle = g_idle_add (paint_cb, data);  
    }  
}  
  
static void  
frame_event_cb (CoglOnscreen *onscreen,  
                CoglFrameEvent event,  
                CoglFrameInfo *info,  
                void *user_data)  
{  
    Data *data = user_data;  
  
    if (event == COGL_FRAME_EVENT_SYNC) {  
        data->frame_sync_ready = TRUE;  
        maybe_queue_redraw (data);  
    }  
}  
  
static void  
dirty_cb (CoglOnscreen *onscreen,  
          const CoglOnscreenDirtyInfo *info,  
          void *user_data)  
{  
    Data *data = user_data;  
  
    data->is_dirty = TRUE;  
    maybe_queue_redraw (data);  
}  
  
int  
main (int argc, char **argv)  
{  
    Data data;  
    CoglError *error = NULL;  
    CoglVertexP2C4 triangle_vertices[] = {  
        {0, 0.7, 0xff, 0x00, 0x00, 0x80},  
        {-0.7, -0.7, 0x00, 0xff, 0x00, 0xff},  
        {0.7, -0.7, 0x00, 0x00, 0xff, 0xff}  
    };  
    GSource *cogl_source;  
    GMainLoop *loop;  
  
    data.redraw_idle = 0;  
    data.is_dirty = FALSE;  
    data.frame_sync_ready = TRUE;  
  
    data.ctx = cogl_context_new (NULL, &error);  
    if (!data.ctx) {  
        fprintf (stderr, "Failed to create context: %s\n", error->message);  
        return 1;  
    }  
  
    data.fb = cogl_onscreen_new (data.ctx, 640, 480);  
    cogl_onscreen_show (data.fb);  
  
    data.triangle = cogl_primitive_new_p2c4 (data.ctx,  
                                             COGL_VERTICES_MODE_TRIANGLES,  
                                             3, triangle_vertices);  
    data.pipeline = cogl_pipeline_new (data.ctx);  
  
    cogl_source = cogl_glib_source_new (data.ctx, G_PRIORITY_DEFAULT);  
  
    g_source_attach (cogl_source, NULL);  
  
    cogl_onscreen_add_frame_callback (data.fb,  
                                      frame_event_cb,  
                                      &data,  
                                      NULL /* destroy callback */);  
  
    cogl_onscreen_add_dirty_callback (data.fb,  
                                      dirty_cb,  
                                      &data,  
                                      NULL /* destroy callback */);  
  
    g_idle_add (paint_cb, &data);  
  
    loop = g_main_loop_new (NULL, TRUE);  
    g_main_loop_run (loop);  
  
    return 0;  
}  
