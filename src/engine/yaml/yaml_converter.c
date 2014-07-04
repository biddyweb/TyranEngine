#include <yaml.h>
#include <tyranscript/tyran_log.h>

int yaml_convert(const char* input, size_t size)
{
	yaml_parser_t parser;
	yaml_event_t  event;

	if(!yaml_parser_initialize(&parser)) {
		TYRAN_ERROR("Couldn't initialize yaml parser");
		return 0;
	}

	yaml_parser_set_input_string(&parser, (const unsigned char*)input, size);

	do {
		if (!yaml_parser_parse(&parser, &event)) {
			TYRAN_ERROR("Parser error %d", parser.error);
			return 0;
		}

		switch(event.type) {
			case YAML_NO_EVENT:
				TYRAN_LOG("No event!");
				break;
			case YAML_STREAM_START_EVENT:
				TYRAN_LOG("STREAM START");
				break;
			case YAML_STREAM_END_EVENT:
				TYRAN_LOG("STREAM END");
				break;
			case YAML_DOCUMENT_START_EVENT:
				TYRAN_LOG("<b>Start Document</b>");
				break;
			case YAML_DOCUMENT_END_EVENT:
				TYRAN_LOG("<b>End Document</b>");
				break;
			case YAML_SEQUENCE_START_EVENT:
				TYRAN_LOG("<b>Start Sequence</b>");
				break;
			case YAML_SEQUENCE_END_EVENT:
				TYRAN_LOG("<b>End Sequence</b>");
				break;
			case YAML_MAPPING_START_EVENT:
				TYRAN_LOG("<b>Start Mapping</b>");
				break;
			case YAML_MAPPING_END_EVENT:
				TYRAN_LOG("<b>End Mapping</b>");
				break;
			case YAML_ALIAS_EVENT:
				TYRAN_LOG("Got alias (anchor %s)", event.data.alias.anchor);
				break;
			case YAML_SCALAR_EVENT:
				TYRAN_LOG("Got scalar (value %s)", event.data.scalar.value);
				break;
		}
		if(event.type != YAML_STREAM_END_EVENT) {
			yaml_event_delete(&event);
		}
	} while(event.type != YAML_STREAM_END_EVENT);

	yaml_event_delete(&event);

	yaml_parser_delete(&parser);

	return 0;
}
