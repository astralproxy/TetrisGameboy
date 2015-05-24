#include "TetrisGameboy.h"
#include "math.h"

static struct Star* create_star()
{
    struct Star *this = malloc(sizeof(struct Star));

    this->x = SCREEN_WIDTH / 2;
    this->y = SCREEN_HEIGHT / 2;

    this->radius = 1;
    this->visible = false;
	
    return this;
}

static void prepare_stars()
{
    mVisibleStars = 0;

    for (int i = 0; i < MAX_STARS; i++)
    {
        stars[i] = create_star();
    }
}

static void destroy_stars()
{
	for (int i = 0; i < MAX_STARS; i++)
	{
		free(stars[i]);
	}
}

static void SpawnStar(int index)
{
    struct Star* s = stars[index];

    // Initialize position of the star anywhere on screen, offset by size of the time display to avoid overlap
    s->x = (rand() % (SCREEN_CENTER_X - STAR_OFFSET_X)); // half screen
    s->x += (rand() % 1 == 0) ? 1 : SCREEN_CENTER_X; // flip sides?
    s->y = (rand() % (SCREEN_CENTER_Y - STAR_OFFSET_Y)); // half screen
    s->y += (rand() % 1 == 0) ? 1 : SCREEN_CENTER_Y; // flip sides?

    // Set the constant normalized velocity for the star
    float magnitude = math_dist2d(SCREEN_CENTER_X, SCREEN_CENTER_Y, s->x, s->y);
    s->velx = s->x / magnitude;
    s->vely = s->y / magnitude;
    APP_LOG(APP_LOG_LEVEL_INFO, "SpawnStar: pos: %d, %d", s->x, s->y);
//" -- velocity %f, %f", s->velx, s->vely
    s->radius = STAR_SIZE_START;

    s->visible = true;
    mVisibleStars++;
}

static void update()
{
    for (int i = 0; i < MAX_STARS; i++)
	{
        struct Star* s = stars[i];
		// Evaluate visibility
		if ((s->x >= SCREEN_WIDTH + s->radius) || (s->x <= -s->radius) ||
            (s->y >= SCREEN_HEIGHT + s->radius) || (s->y <= -s->radius))
        {
            s->visible = false;
            mVisibleStars--;
        }
		
		// Update visible
		if (s->visible == true)
		{
            // Move the star
            s->x += s->velx * mStarSpeed;
            s->y += s->vely * mStarSpeed;
	              
            // Increase size as the star gets "closer"
            s->radius = math_min(math_dist2d(0, 0, s->x, s->y) / DISTANCE_TO_GROW, STAR_SIZE_MAX);
		}
	}

	// Spawn new stars (per frame)
	if (mVisibleStars < MAX_STARS)
	{
        for (int a = 0; a < STARS_PER_FRAME; a++)
        {
    		// Find next slot
    		for (int b = 0; b < MAX_STARS; b++)
    		{
    			if (stars[b]->visible == false)
    			{
                    if (b < MAX_STARS)
            		{
                        SpawnStar(b);
            		}
    
    				break;
    			}
            }
		}		
	}
}

static void timer_callback(void *data)
{
	update();
	layer_mark_dirty(canvas);
	timer = app_timer_register(mFramerate, (AppTimerCallback) timer_callback, 0);
}

static void start()
{
	timer = app_timer_register(mFramerate, (AppTimerCallback) timer_callback, 0);
}

static void render(Layer *layer, GContext* ctx) 
{
	graphics_context_set_fill_color(ctx, mForeColor);

	// Draw all stars
	for (int i = 0; i < MAX_STARS; i++)
	{
		if (stars[i]->visible == true)
		{
			graphics_fill_rect(ctx, GRect(stars[i]->x, stars[i]->y, stars[i]->radius, stars[i]->radius), 0, GCornerNone);
		}
	}
	
	// Draw time
	graphics_context_set_text_color(ctx, mForeColor);
	graphics_draw_text(ctx, mTimeText, mFont, GRect(TIME_X, TIME_Y, TIME_WIDTH, TIME_HEIGHT), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void set_time_display(struct tm *t)
{
	int size = sizeof("00:00");

	if (mTimeText == NULL)
	{
		mTimeText = malloc(size);
	}
	
	if (clock_is_24h_style())
	{
		strftime(mTimeText, size, "%H:%M", t);
	}
	else
	{
		strftime(mTimeText, size, "%I:%M", t);
	}
}

static void window_load(Window *window)
{
    // Check for saved settings
    mInvertColors = persist_exists(KEY_INVERT) ? persist_read_bool(KEY_INVERT) : false;
    mBackColor = (mInvertColors) ? GColorWhite : GColorBlack;
    mForeColor = (mInvertColors) ? GColorBlack : GColorWhite;

    mStarSpeed = STAR_SPEED_SLOW;//persist_exists(KEY_STAR_SPEED) ? persist_read_int(KEY_STAR_SPEED) : STAR_SPEED_FAST;
    mFramerate = FRAMERATE_SMOOTH;//persist_exists(KEY_FRAMERATE) ? persist_read_int(KEY_FRAMERATE) : FRAMERATE_POWER;

	window_set_background_color(window, mBackColor);
	canvas = layer_create(GRect(0, 0, SCREEN_WIDTH , SCREEN_HEIGHT));
	layer_set_update_proc(canvas, (LayerUpdateProc) render);
	layer_add_child(window_get_root_layer(window), canvas);
    
    mFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TETRIS_24));
	
	// Set initial time so display isn't blank
    mTimeText = NULL;
	struct tm *t;
	time_t temp;
	temp = time(NULL);
	t = localtime(&temp);
	set_time_display(t);

	start();
}

static void window_unload(Window *window) 
{
    app_timer_cancel(timer);
    fonts_unload_custom_font(mFont);
    layer_destroy(canvas);
}

static void tick_handler(struct tm *t, TimeUnits units_changed)
{
	set_time_display(t);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
    Tuple* t = dict_read_first(iterator);
    while (t != NULL)
    {
        switch(t->key)
        {
        case KEY_INVERT:
            if (strcmp(t->value->cstring, "off") == 0)
            {
                mInvertColors = false;
            }
            if (strcmp(t->value->cstring, "on") == 0)
            {
                mInvertColors = true;
            }
            persist_write_bool(KEY_INVERT, mInvertColors);
            break;

        case KEY_STAR_SPEED:
            if (strcmp(t->value->cstring, "slow") == 0)
            {
                mStarSpeed = STAR_SPEED_SLOW;
            }
            else if (strcmp(t->value->cstring, "medium") == 0)
            {
                mStarSpeed = STAR_SPEED_MEDIUM;
            }
            else if (strcmp(t->value->cstring, "fast") == 0)
            {
                mStarSpeed = STAR_SPEED_FAST;
            }
            
            persist_write_int(KEY_STAR_SPEED, mStarSpeed);
        break;

        case KEY_FRAMERATE:
            if (strcmp(t->value->cstring, "power") == 0)
            {
                mFramerate = FRAMERATE_POWER;
            }
            else if (strcmp(t->value->cstring, "smooth") == 0)
            {
                mFramerate = FRAMERATE_SMOOTH;
            }
            
            persist_write_int(KEY_FRAMERATE, mFramerate);
        break;
        }
        
        t = dict_read_next(iterator);
    }
}

static void init(void)
{
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) 
    {
		.load = window_load,
		.unload = window_unload,
    });
    
    // Open AppMessage to enable communication with the phone
    app_message_register_inbox_received((AppMessageInboxReceived) inbox_received_callback);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	//Prepare stars memory
	prepare_stars();
	
	//Tick tock
	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
	
	//Finally
	window_stack_push(window, true);
}

static void deinit(void) 
{
	destroy_stars();
	tick_timer_service_unsubscribe();
	window_destroy(window);
}

int main(void) 
{
	init();
	app_event_loop();
	deinit();
}