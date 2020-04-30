Pod::Spec.new do |s|
  IOS_DEPLOYMENT_TARGET = '6.0' unless defined? IOS_DEPLOYMENT_TARGET
  s.name         = "KZCrash"
  s.version      = "1.15.25"
  s.summary      = "The Ultimate iOS Crash Reporter"
  s.homepage     = "https://github.com/perrzick/KZCrash"
  s.license     = { :type => 'KZCrash license agreement', :file => 'LICENSE' }
  s.author       = { "Karl Stenerud" => "kstenerud@gmail.com" }
  s.ios.deployment_target =  IOS_DEPLOYMENT_TARGET
  s.osx.deployment_target =  '10.8'
  s.tvos.deployment_target =  '9.0'
  s.watchos.deployment_target =  '2.0'
  s.source       = { :git => "https://github.com/perrzick/KZCrash.git", :tag=>s.version.to_s }
  s.frameworks = 'Foundation'
  s.libraries = 'c++', 'z'
  s.xcconfig = { 'GCC_ENABLE_CPP_EXCEPTIONS' => 'YES' }
  s.default_subspecs = 'Installations'

  s.subspec 'Recording' do |recording|
    recording.source_files   = 'Source/KSCrash/Recording/**/*.{h,m,mm,c,cpp}',
                               'Source/KSCrash/llvm/**/*.{h,m,mm,c,cpp}',
                               'Source/KSCrash/swift/**/*.{h,m,mm,c,cpp}',
                               'Source/KSCrash/Reporting/Filters/KSCrashReportFilter.h'
    recording.public_header_files = 'Source/KSCrash/Recording/KSCrash.h',
                                    'Source/KSCrash/Recording/KSCrashC.h',
                                    'Source/KSCrash/Recording/KSCrashReportWriter.h',
                                    'Source/KSCrash/Recording/KSCrashReportFields.h',
                                    'Source/KSCrash/Recording/Monitors/KSCrashMonitorType.h',
                                    'Source/KSCrash/Reporting/Filters/KSCrashReportFilter.h'

    recording.subspec 'Tools' do |tools|
      tools.source_files = 'Source/KSCrash/Recording/Tools/*.h'
    end
  end

  s.subspec 'Reporting' do |reporting|
    reporting.dependency 'KZCrash/Recording'

    reporting.subspec 'Filters' do |filters|
      filters.subspec 'Base' do |base|
        base.source_files = 'Source/KSCrash/Reporting/Filters/Tools/**/*.{h,m,mm,c,cpp}',
                            'Source/KSCrash/Reporting/Filters/KSCrashReportFilter.h'
        base.public_header_files = 'Source/KSCrash/Reporting/Filters/KSCrashReportFilter.h'
      end

      filters.subspec 'Alert' do |alert|
        alert.dependency 'KZCrash/Reporting/Filters/Base'
        alert.source_files = 'Source/KSCrash/Reporting/Filters/KSCrashReportFilterAlert.h',
                             'Source/KSCrash/Reporting/Filters/KSCrashReportFilterAlert.m'
      end

      filters.subspec 'AppleFmt' do |applefmt|
        applefmt.dependency 'KZCrash/Reporting/Filters/Base'
        applefmt.source_files = 'Source/KSCrash/Reporting/Filters/KSCrashReportFilterAppleFmt.h',
                             'Source/KSCrash/Reporting/Filters/KSCrashReportFilterAppleFmt.m'
      end

      filters.subspec 'Basic' do |basic|
        basic.dependency 'KZCrash/Reporting/Filters/Base'
        basic.source_files = 'Source/KSCrash/Reporting/Filters/KSCrashReportFilterBasic.h',
                             'Source/KSCrash/Reporting/Filters/KSCrashReportFilterBasic.m'
      end

      filters.subspec 'Stringify' do |stringify|
        stringify.dependency 'KZCrash/Reporting/Filters/Base'
        stringify.source_files = 'Source/KSCrash/Reporting/Filters/KSCrashReportFilterStringify.h',
                                 'Source/KSCrash/Reporting/Filters/KSCrashReportFilterStringify.m'
      end

      filters.subspec 'GZip' do |gzip|
        gzip.dependency 'KZCrash/Reporting/Filters/Base'
        gzip.source_files = 'Source/KSCrash/Reporting/Filters/KSCrashReportFilterGZip.h',
                            'Source/KSCrash/Reporting/Filters/KSCrashReportFilterGZip.m'
      end

      filters.subspec 'JSON' do |json|
        json.dependency 'KZCrash/Reporting/Filters/Base'
        json.source_files = 'Source/KSCrash/Reporting/Filters/KSCrashReportFilterJSON.h',
                            'Source/KSCrash/Reporting/Filters/KSCrashReportFilterJSON.m'
      end

      filters.subspec 'Sets' do |sets|
        sets.dependency 'KZCrash/Reporting/Filters/Base'
        sets.dependency 'KZCrash/Reporting/Filters/AppleFmt'
        sets.dependency 'KZCrash/Reporting/Filters/Basic'
        sets.dependency 'KZCrash/Reporting/Filters/Stringify'
        sets.dependency 'KZCrash/Reporting/Filters/GZip'
        sets.dependency 'KZCrash/Reporting/Filters/JSON'

        sets.source_files = 'Source/KSCrash/Reporting/Filters/KSCrashReportFilterSets.h',
                            'Source/KSCrash/Reporting/Filters/KSCrashReportFilterSets.m'
      end

      filters.subspec 'Tools' do |tools|
        tools.source_files = 'Source/KSCrash/Reporting/Filters/Tools/**/*.{h,m,mm,c,cpp}'
      end

    end

    reporting.subspec 'Tools' do |tools|
      tools.ios.frameworks = 'SystemConfiguration'
      tools.tvos.frameworks = 'SystemConfiguration'
      tools.osx.frameworks = 'SystemConfiguration'
      tools.source_files = 'Source/KSCrash/Reporting/Tools/**/*.{h,m,mm,c,cpp}',
                           'Source/KSCrash/Recording/KSSystemCapabilities.h'
    end

    reporting.subspec 'MessageUI' do |messageui|
    end

    reporting.subspec 'Sinks' do |sinks|
      sinks.ios.frameworks = 'MessageUI'
      sinks.dependency 'KZCrash/Reporting/Filters'
      sinks.dependency 'KZCrash/Reporting/Tools'
      sinks.source_files = 'Source/KSCrash/Reporting/Sinks/**/*.{h,m,mm,c,cpp}'
    end

  end

  s.subspec 'Installations' do |installations|
    installations.dependency 'KZCrash/Recording'
    installations.dependency 'KZCrash/Reporting'
    installations.source_files = 'Source/KSCrash/Installations/**/*.{h,m,mm,c,cpp}'
  end

  s.subspec 'Core' do |core|
    core.dependency 'KZCrash/Reporting/Filters/Basic'
    core.source_files = 'Source/KSCrash/Installations/KSCrashInstallation.h',
                        'Source/KSCrash/Installations/KSCrashInstallation.m',
                        'Source/KSCrash/Installations/KSCrashInstallation+Private.h',
                        'Source/KSCrash/Reporting/Tools/KSCString.{h,m}'
  end

end
