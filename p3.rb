#!/usr/bin/env ruby

require 'socket'

class HTTP_lol
  def initialize(port,path)
    @webserver=TCPServer.new('127.0.0.1',port)
    @path=path
    @server="Server: Ruby/#{RUBY_VERSION}@#{RUBY_PLATFORM}"
  end
  def destroy
    @webserver.close
  end
  def accept
    begin
      @session=@webserver.accept_nonblock
      1
    rescue Errno::EAGAIN
      0
    end
  end
  def finish
    @session.close
  end
  def trim(msg)
    File.expand_path(msg.gsub(/GET\ \//,'').gsub(/\ HTTP.*/,'').chomp,@path)
  end
  def type(file)
    'Content-type:'+
    case File.extname(file)
      when '.html' then 'text/html'
      when '.htm' then 'text/html'
      when '.txt' then 'text/plain'
      when '.css' then 'text/css'
      when '.jpg' then 'image/jpeg'
      when '.jpeg' then 'image/jpeg'
      when '.gif' then 'image/gif'
      when '.bmp' then 'image/bmp'
      when '.xml' then 'text/xml'
      when '.xsl' then 'text/xsl'
      else 'text/plain'
    end
  end
  def size(file)
    'Content-length:'+File.size(file).to_s
  end
  def serve
    while result=IO.select([@session],nil,nil,5)
      request=@session.gets if !result.nil?
      if request=~/HTTP\/1.1/
        if !request.include? "GET"
          @session.print "HTTP/1.1 501 Not Implemented\r\n#{@server}\r\n\r\n"
        else
          file=trim(request)
          if !File.exist?(file) or file.index(@path)!=0
            @session.print "HTTP/1.1 404 Not Found\r\n#{@server}\r\n\r\n"
          else
            if !File.exist?(file)
              @session.print "HTTP/1.1 404 Not Found\r\n#{@server}\r\n\r\n"
            else
              if File.directory?(file)
                file<<'/index.html'
                @session.print "HTTP/1.1 302 Found\r\n#{@server}\r\n#{type(file)}\r\n#{size(file)}\r\n\r\n"
              else
                @session.print "HTTP/1.1 200/OK\r\n#{@server}\r\n#{type(file)}\r\n#{size(file)}\r\n\r\n"
              end
              show(file)
            end
          end
        end
      elsif request=~/Connection: Close/i
        break
      end
    end
    finish
  end
  def show(file)
    fp=File.open(file,'r')
    @session.print fp.read
    fp.close
  end
  private :trim,:type,:size,:show
end

def quit?
  begin
    while c=STDIN.read_nonblock(2)
      return true if c==':q'
    end
    false
  rescue EOFError
    true
  rescue Exception
    false
  end
end

if ARGV.length!=2
  puts 'Give me parameters! I need two of them! Really!'
else
  worker=HTTP_lol.new(ARGV[0],ARGV[1])
  puts "Welcome Stranger.","This is tiny HTTP server running Ruby #{RUBY_VERSION} on #{RUBY_PLATFORM}.","Server is now working.","Type ':q' or hit CTRL-D to exit."
  while not quit?
    if (worker.accept)>0 then
      pid=fork do
        worker.serve
        worker.destroy
      end
      worker.finish
      Process.detach(pid)
    end
  end
  worker.destroy
  puts "\nThanks for using me. ;)"
end
