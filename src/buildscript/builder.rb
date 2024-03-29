require 'rake'
require 'rake/clean'

class Builder
	include Rake::DSL

	def initialize application, includes
		@includes = includes

		build_directory = 'build/'
		p "directory"
		directory build_directory

		c_files = FileList['./**/*.c', 'tyranscript/src/lib/**/*.c']
		p "c:#{c_files.inspect}"

		o_files = []
		c_files.each do |source_file|
			object_file = File.join build_directory, File.basename(source_file).ext('o')
			o_files << object_file

			file object_file => source_file do |f|
				p "f:#{f.to_s}"
				compile source_file, object_file
			end
		end

		file application => o_files do
			link o_files, application
		end

		p "o:#{o_files.inspect}"
		@c_compiler = 'clang'
		@c_compiler = 'gcc.exe'

		CLEAN.include o_files
	end

	private

	def parameter_string prefix, array
		s = ''
		array.each do |a|
			s += " -#{prefix} #{a}"
		end
		s
	end

	def link object_files, application
		puts "* Link #{application}"
		@libraries = ['m', 'ws2_32']
		sh "#{@c_compiler} #{object_files.join(' ')} #{parameter_string('l', @libraries)} -o #{application}"
	end

	def compile source, target
		p "compile #{source.inspect}"
		c_flags = '-g -O0'
		# c_flags = '-O3'

		sh "#{@c_compiler} -c #{source} #{c_flags} -Wall -pedantic -Werror #{parameter_string('I', @includes)} -std=c99 -o #{target} -D TORNADO_OS_WINDOWS"
	end
end

