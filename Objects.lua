function table.HasValue(table, value, index)
  index = index or nil
    if index then
      for _, v in pairs(table) do
        if v[index] == value then
          return true
        end
      end
      return false
    else
      for _, v in pairs(table) do
        if v == value then
          return true
        end
      end
      return false
    end
  end
  
  function string.strsplit (inputstr, sep)
    if sep == nil then
            sep = "%s"
    end
    local t={}
    for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
            table.insert(t, str)
    end
    return t
end

function string.split(inputstr, sep)
  if sep == nil then
      sep = "%s"
  end
  local t={}
  for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
      table.insert(t, str)
  end
  return t
end

if not unpack then
  unpack = table.unpack
end

function fwrap (f,co)
  local obj = {}
  local started
  function obj:read ()
      if not started then
          f:read_async(co)
          started = true
      end
      return yield()
  end
  function obj:write (s)
      return f:write(s)
  end
  return obj
end